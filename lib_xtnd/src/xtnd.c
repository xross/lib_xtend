#include "xtnd.h"

#if defined(XTND_BASE_ADDR)
#define BASE_ADDR XTND_BASE_ADDR
#else
#define BASE_ADDR XS1_RAM_BASE
#endif

/* Runs contructors from ctors table
 * Returns number run, or -1 on error
 */
static int xtnd_run_ctors(const xtnd_table_t *t)
{
    if (!t || !t->ctors_base)
    {
        return -1;
    }

    int ran = 0;
    for (uint32_t i = 0; i < t->ctors_count; ++i)
    {
        uint32_t off = ((const uint32_t*)t->ctors_base)[i] - BASE_ADDR;

        /* Check constuctor table offset is valid (word aligned and within the blob) */
        if (off & 3u)
        {
            return -1;
        }
        if (off >= t->hdr->init_len)
        {
            return -1;
        }
        void *fn = (void *)(t->blob_base + off);
        (void)xtnd_call(t, fn, 0, 0);
        ran++;
    }
    return ran;
}

xtnd_status_t xtnd_free(xtnd_table_t *t)
{
    /* TODO Free allocated lock */

    return XTND_OK;
}

xtnd_status_t xtnd_init(uint8_t *blob, size_t maxSize, xtnd_table_t *t)
{
    if (!blob || !t)
        return XTND_ERR_INPUT;

    const xtnd_header_t *h = (const xtnd_header_t *)blob;

#ifdef XTND_OUTPUT_SYSCALL_ADDR
    printhexln(blob + h->syscall_off);
#endif

    if (h->magic != XTND_EXPORT_MAGIC)
        return XTND_ERR_MAGIC;
    if (h->version != XTND_EXPORT_VERSION)
        return XTND_ERR_VERSION;

    const uint32_t fn_count = h->fn_count;

    if (fn_count > (h->init_len - sizeof(*h)) / (sizeof(uint32_t) * 2u))
        return XTND_ERR_COUNT;

    size_t arrays_bytes = (size_t)fn_count * sizeof(uint32_t) * 2u;
    size_t prefix = sizeof(*h) + arrays_bytes;
    if (prefix > h->init_len)
        return XTND_ERR_LAYOUT;

    if ((h->cp_off & 3u) || (h->dp_off & 3u))
        return XTND_ERR_CPDP_MIS;

    if (h->cp_off > h->init_len || h->dp_off > h->init_len)
        return XTND_ERR_CPDP_OOB;

    t->hdr         = h;
    t->fn_offs     = (const uint32_t *)(h + 1);
    t->name_offs   = t->fn_offs + fn_count;
    t->strings     = (const char *)blob + prefix;
    t->strings_len = h->init_len - prefix;
    t->blob_base   = blob;
    t->cp_base     = blob + h->cp_off;
    t->dp_base     = blob + h->dp_off;
    t->ctors_base  = blob + h->ctors_off;
    t->ctors_count = (h->ctors_end_off - h->ctors_off) / sizeof(uint32_t);

    /* Optional timers table */
    if (h->timers_off && h->timers_off < h->init_len && (h->timers_off % 4u) == 0)
    {
        t->timers_base = blob + h->timers_off;
    }
    else
    {
        t->timers_base = NULL;
    }

    if(maxSize < h->mem_len)
    {
        printf("[xtnd] mem available: %d bytes. Required: %d bytes\n", (int)maxSize, (int)h->mem_len);
        return XTND_ERR_SIZE;
    }

    /* Zero the bss section */
    for (int i = 0; i < (int)(h->mem_len - h->init_len); i++)
    {
        t->blob_base[h->init_len + i] = 0;
    }

    int ctors_ran = xtnd_run_ctors(t);

    if (ctors_ran < 0)
        return XTND_ERR_CTORS;

    return XTND_OK;
}

void *xtnd_find(const xtnd_table_t *t, const char *name)
{
    if (!t || !name)
    {
        return NULL;
    }

    uint32_t fn_count = t->hdr->fn_count;
    for (uint32_t i = 0; i < fn_count; ++i)
    {
        uint32_t fn_off = t->fn_offs[i];
        uint32_t nm_off = t->name_offs[i];


        if ((fn_off & 3u) || fn_off >= t->hdr->init_len|| nm_off >= t->hdr->init_len)
        {
            continue;
        }

        const char *nm = (const char *)t->blob_base + nm_off;
        size_t remain = t->hdr->init_len - nm_off;
        size_t slen = strnlen(nm, remain);
        if (slen == remain)
        {
            continue;
        }

        if (strcmp(nm, name) == 0)
        {
            return (void *)(t->blob_base + fn_off);
        }
    }
    return NULL;
}

int xtnd_read(fl_QSPIPorts spiPort, uint8_t *blob_space, size_t blob_space_size)
{
    /* Check we have enough space for at least the xtnd header */
    if(blob_space_size < sizeof(xtnd_header_t))
    {
        printf("[xtnd] ERROR: MAX_XTND_BLOB_SIZE %d is smaller than xtnd_header_t %d\n",
               blob_space_size, (int)sizeof(xtnd_header_t));
        return 1;
    }

    /* Connect to the QSPI flash */
    int fail = fl_connect(&spiPort);

    if (fail)
        return fail;

    /* Read enough data for the header */
    fail = fl_readData(0, sizeof(xtnd_header_t), blob_space);

    if (fail)
        return fail;

    const xtnd_header_t *h = (const xtnd_header_t *) blob_space;
    printf("[xtnd] blob size = %lu bytes\n", h->init_len);

    /* Check the data partition  is actually large enough to store the reported blob lenght */
    int dataPartitionSize_bytes = fl_getDataPartitionSize();
    if(dataPartitionSize_bytes < h->init_len)
    {
        printf("[xtnd] ERROR: Data partition size %d bytes is smaller than blob size %lu bytes\n",
               dataPartitionSize_bytes, h->init_len);
        return 1;
    }

    if(h->init_len > blob_space_size)
    {
        printf("[xtnd] ERROR: MAX_XTND_BLOB_SIZE %d is smaller than blob size %lu bytes\n",
               blob_space_size, h->init_len);
        return 1;
    }

    /* Read the whole blob */
    fail = fl_readData(0, h->init_len, blob_space);

    if (fail)
        return fail;

    return 0;
}
