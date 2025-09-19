#include "xtend.h"

#include "xtend_offsets.h"
/* Runs contructors from ctors table
 * Returns number run, or -1 on error
 */
static int xtend_run_ctors(const xtend_table_t *t)
{
    if (!t || !t->ctors_base)
    {
        return -1;
    }

    int ran = 0;
    for (uint32_t i = 0; i < t->ctors_count; ++i)
    {
        uint32_t off = ((const uint32_t*)t->ctors_base)[i] - XS1_RAM_BASE;

        if ((off & 3u) || off >= t->hdr->length)
        {
            return -1;
        }
        void *fn = (void *)(t->blob_base + off);
        (void)xtend_call(t, fn, 0, 0);
        ran++;
    }
    return ran;
}

xtend_status_t xtend_init(const uint8_t *blob, xtend_table_t *t)
{
    if (!blob || !t)
        return XTEND_ERR_INPUT;

    const xtend_header_t *h = (const xtend_header_t *)blob;

#ifdef XTEND_OUTPUT_SYSCALL_ADDR
    printhexln(blob + h->syscall_off);
#endif

    if (h->magic != XTEND_EXPORT_MAGIC)
        return XTEND_ERR_MAGIC;
    if (h->version != XTEND_EXPORT_VERSION)
        return XTEND_ERR_VERSION;

    uint32_t count = h->count;
    if (count > (h->length - sizeof(*h)) / (sizeof(uint32_t) * 2u))
        return XTEND_ERR_COUNT;

    size_t arrays_bytes = (size_t)count * sizeof(uint32_t) * 2u;
    size_t prefix = sizeof(*h) + arrays_bytes;
    if (prefix > h->length)
        return XTEND_ERR_LAYOUT;

    if ((h->cp_off & 3u) || (h->dp_off & 3u))
        return XTEND_ERR_CPDP_MIS;

    if (h->cp_off > h->length || h->dp_off > h->length)
        return XTEND_ERR_CPDP_OOB;

    t->hdr         = h;
    t->fn_offs     = (const uint32_t *)(h + 1);
    t->name_offs   = t->fn_offs + count;
    t->strings     = (const char *)blob + prefix;
    t->strings_len = h->length - prefix;
    t->blob_base   = blob;
    t->cp_base     = blob + h->cp_off;
    t->dp_base     = blob + h->dp_off;
    t->ctors_base  = blob + h->ctors_off;
    t->ctors_count = (h->ctors_end_off - h->ctors_off) / sizeof(uint32_t);

    int ctors_ran = xtend_run_ctors(t);

    if (ctors_ran < 0)
        return XTEND_ERR_CTORS;

    return XTEND_OK;
}

void *xtend_find(const xtend_table_t *t, const char *name)
{
    if (!t || !name)
        return NULL;

    uint32_t count = t->hdr->count;
    for (uint32_t i = 0; i < count; ++i)
    {
        uint32_t fn_off = t->fn_offs[i];
        uint32_t nm_off = t->name_offs[i];
        if ((fn_off & 3u) || fn_off >= t->hdr->length|| nm_off >= t->hdr->length)
            continue;

        const char *nm = (const char *)t->blob_base + nm_off;
        size_t remain = t->hdr->length - nm_off;
        size_t slen = strnlen(nm, remain);
        if (slen == remain)
            continue;
        if (strcmp(nm, name) == 0)
            return (void *)(t->blob_base + fn_off);
    }
    return NULL;
}

int xtend_read(fl_QSPIPorts spiPort, uint8_t *blob_space, size_t blob_space_size)
{
    /* Check we have enough space for at least the xtend header */
    if(blob_space_size < sizeof(xtend_header_t))
    {
        printf("[xtend] ERROR: MAX_XTEND_BLOB_SIZE %d is smaller than xtend_header_t %d\n",
               blob_space_size, (int)sizeof(xtend_header_t));
        return 1;
    }

    /* Connect to the QSPI flash */
    int fail = fl_connect(&spiPort);

    if (fail)
        return fail;

    /* Read enough data for the header */
    fail = fl_readData(0, sizeof(xtend_header_t), blob_space);

    if (fail)
        return fail;

    const xtend_header_t *h = (const xtend_header_t *) blob_space;
    printf("[xtend] blob size = %lu bytes\n", h->length);

    /* Check the data paritiin is actually large enough to store the reported blob lenght */
    int dataPartitionSize_bytes = fl_getDataPartitionSize();
    if(dataPartitionSize_bytes < h->length)
    {
        printf("[xtend] ERROR: Data partition size %d bytes is smaller than blob size %lu bytes\n",
               dataPartitionSize_bytes, h->length);
        return 1;
    }

    /* Read the whole blob */
    fail = fl_readData(0, h->length, blob_space);

    if (fail)
        return fail;

    return 0;
}

