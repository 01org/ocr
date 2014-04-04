/**
 * @brief OCR internal API to GUID management
 **/

/*
 * This file is subject to the license agreement located in the file LICENSE
 * and cannot be distributed without it. This notice cannot be
 * removed or modified.
 */


#ifndef __OCR_GUID_H__
#define __OCR_GUID_H__

#include "ocr-runtime-types.h"
#include "ocr-types.h"
#include "utils/ocr-utils.h"

typedef enum {
    OCR_GUID_NONE = 0,
    OCR_GUID_ALLOCATOR = 1,
    OCR_GUID_DB = 2,
    OCR_GUID_EDT = 3,
    OCR_GUID_EDT_TEMPLATE = 4,
    OCR_GUID_EVENT = 5,
    OCR_GUID_POLICY = 6,
    OCR_GUID_WORKER = 7,
    OCR_GUID_MEM_TARGET = 8,
    OCR_GUID_COMP_TARGET = 9
} ocrGuidKind;


/****************************************************/
/* OCR PARAMETER LISTS                              */
/****************************************************/

/**
 * @brief Parameter list to create a guid provider factory
 */
typedef struct _paramListGuidProviderFact_t {
    ocrParamList_t base;
} paramListGuidProviderFact_t;

/**
 * @brief Parameter list to create a guid provider instance
 */
typedef struct _paramListGuidProviderInst_t {
    ocrParamList_t base;
} paramListGuidProviderInst_t;


/****************************************************/
/* OCR GUID PROVIDER                                */
/****************************************************/

struct _ocrGuidProvider_t;
struct _ocrPolicyDomain_t;

/**
 * @brief GUID provider function pointers
 *
 * The function pointers are separate from the GUID provider instance to allow
 * for the sharing of function pointers for GUID provider from the same factory
 */
typedef struct _ocrGuidProviderFcts_t {
    /**
     * @brief Destructor equivalent
     *
     * This will free the GUID provider and any
     * memory that it uses
     *
     * @param self          Pointer to this GUID provider
     */
    void (*destruct)(struct _ocrGuidProvider_t* self);

    void (*begin)(struct _ocrGuidProvider_t *self, struct _ocrPolicyDomain_t *pd);

    /**
     * @brief "Starts" the GUID provider
     *
     * @param[in] self      Pointer to this GUID provider
     * @param[in] pd        Policy domain this provider belongs to
     */
    void (*start)(struct _ocrGuidProvider_t* self, struct _ocrPolicyDomain_t* pd);

    void (*stop)(struct _ocrGuidProvider_t* self);
    void (*finish)(struct _ocrGuidProvider_t* self);

    /**
     * @brief Gets a GUID for an object of kind 'kind'
     * and associates the value val.
     *
     * The GUID provider basically associates a value with the
     * GUID
     *
     * @param[in] self          Pointer to this GUID provider
     * @param[out] guid         GUID returned
     * @param[in] val           Value to be associated
     * @param[in] kind          Kind of the object that will be associated with the GUID
     * @return 0 on success or an error code
     */
    u8 (*getGuid)(struct _ocrGuidProvider_t* self, ocrGuid_t* guid, u64 val,
                  ocrGuidKind kind);

    /**
     * @brief Create a GUID for an object of kind 'kind'
     * and creates storage of size 'size' associated with the
     * GUID
     *
     * getGuid() will associate an existing 64-bit value to a
     * GUID but createGuid() will create storage of size 'size'
     * and associate the resulting address with a GUID. This
     * is useful to create metadata storage
     *
     *
     * @param[in] self          Pointer to this GUID provider
     * @param[out] fguid        GUID returned (with metaDataPtr)
     * @param[in] size          Size of the storage to be created
     * @param[in] kind          Kind of the object that will be associated with the GUID
     * @return 0 on success or an error code
     */
    u8 (*createGuid)(struct _ocrGuidProvider_t* self, ocrFatGuid_t* fguid,
                     u64 size, ocrGuidKind kind);

    /**
     * @brief Resolve the associated value to the GUID 'guid'
     *
     * \param[in] self          Pointer to this GUID provider
     * \param[in] guid          GUID to resolve
     * \param[out] val          Parameter-result for the value to return
     * \param[out] kind         Parameter-result for the GUID's kind. Can be NULL if the user does not care about the kind
     *
     * @return 0 on success or an error code
     */
    u8 (*getVal)(struct _ocrGuidProvider_t* self, ocrGuid_t guid, u64* val, ocrGuidKind* kind);

    /**
     * @brief Resolve the kind of a GUID
     *
     * \param[in] self          Pointer to this GUID provider
     * \param[in] guid          GUID to get the kind of
     * \param[out] kind         Parameter-result for the GUID's kind.
     * @return 0 on success or an error code
     */
    u8 (*getKind)(struct _ocrGuidProvider_t* self, ocrGuid_t guid, ocrGuidKind* kind);

    /**
     * @brief Releases the GUID
     *
     * Whether the GUID provider will re-issue this same GUID for a different
     * object is implementation dependent.
     *
     * @param[in] self        Pointer to this GUID provider
     * @param[in] guid        GUID to release
     * @param[in] releaseVal  If true, will also "free" the value associated
     *                        with the GUID
     * @return 0 on success or an error code
     */
    u8 (*releaseGuid)(struct _ocrGuidProvider_t *self, ocrFatGuid_t guid, bool releaseVal);
} ocrGuidProviderFcts_t;

/**
 * @brief GUIDs Provider for the system
 *
 * GUIDs should be unique and are used to
 * identify and locate objects (and their associated
 * metadata mostly). GUIDs serve as a level of indirection
 * to allow objects to move around in the system and
 * support different address spaces (in the future)
 */
typedef struct _ocrGuidProvider_t {
    struct _ocrPolicyDomain_t *pd;  /**< Policy domain of this GUID provider */
    u32 id;                         /**< Function IDs for this GUID provider */
    ocrGuidProviderFcts_t fcts;     /**< Functions for this instance */
} ocrGuidProvider_t;


/****************************************************/
/* OCR GUID PROVIDER FACTORY                        */
/****************************************************/

/**
 * @brief GUID provider factory
 */
typedef struct _ocrGuidProviderFactory_t {
    /**
     * @brief Instantiate a new GUID provider and returns a pointer to it.
     *
     * @param factory       Pointer to this factory
     * @param instanceArg   Arguments specific for this instance
     */
    ocrGuidProvider_t* (*instantiate)(struct _ocrGuidProviderFactory_t *factory, ocrParamList_t* instanceArg);

    /**
     * @brief GUID provider factory destructor
     * @param factory       Pointer to the factory to destroy.
     */
    void (*destruct)(struct _ocrGuidProviderFactory_t *factory);

    u32 factoryId;
    ocrGuidProviderFcts_t providerFcts; /**< Function pointers created instances should use */
} ocrGuidProviderFactory_t;

/****************************************************/
/* OCR GUID CONVENIENCE FUNCTIONS                   */
/****************************************************/

/**
 * @brief Resolve the kind of a guid (could be an event, an edt, etc...)
 * @param[in] pd          Policy domain
 * @param[in] guid        The GUID for which we need the kind
 * @param[out] kindRes    Parameter-result to contain the kind
 */
static inline u8 guidKind(struct _ocrPolicyDomain_t * pd, ocrFatGuid_t guid,
                          ocrGuidKind* kindRes) __attribute__((unused));

/**
 *  @brief Generates a GUID based on the value 'val'.
 *
 *  This does not allocate space for the metadata associated with the GUID
 *  but rather associates a GUID with the value passed in.
 *
 *  @param[in] pd          Policy domain
 *  @param[in] ptr         The pointer for which we want a guid
 *  @param[out] guidRes    Parameter-result to contain the guid
 *  @param[in] kind        The kind of the guid (whether is an event, an edt, etc...)
 *
 *  @return 0 on success and a non zero failure on failure
 */
static inline u8 guidify(struct _ocrPolicyDomain_t * pd, u64 val, ocrFatGuid_t * guidRes,
                         ocrGuidKind kind) __attribute__((unused));

/**
 * @brief Resolves the pointer to the metadata out of the GUID
 *
 * Note that the value in metaDataPtr should only be used as a
 * READ-ONLY value. This call may return a COPY of the metadata
 * area.
 * @param[in] pd          Policy domain
 * @param[in/out] res     Parameter-result to contain the fully resolved GUID
 * @param[out] kindRes    Parameter-result to contain the kind
 */
static inline u8 deguidify(struct _ocrPolicyDomain_t * pd, ocrFatGuid_t *res,
                           ocrGuidKind* kindRes) __attribute__((unused));

/**
 * @brief Check if a GUID represents a data-block
 *
 * @param[in] pd          Policy domain in which we are
 * @param[in] guid        The GUID to check the kind
 */
static inline bool isDatablockGuid(struct _ocrPolicyDomain_t *pd,
                                   ocrFatGuid_t guid) __attribute__((unused));

/**
 * @brief Check if a GUID represents an event
 *
 * @param[in] pd          Policy domain in which we are
 * @param[in] guid        The GUID to check the kind
 */
static inline bool isEventGuid(struct _ocrPolicyDomain_t *pd,
                               ocrFatGuid_t guid) __attribute__((unused));

/**
 * @brief Check if a GUID represents an EDT
 *
 * @param[in] pd          Policy domain in which we are
 * @param[in] guid        The GUID to check the kind
 */
static inline bool isEdtGuid(struct _ocrPolicyDomain_t *pd,
                             ocrFatGuid_t guid) __attribute__((unused));

/*! \brief Check if a guid represents a latch-event
 *  \param[in] guid        The guid to check the kind
 */
static inline ocrEventTypes_t eventType(struct _ocrPolicyDomain_t *pd,
                                        ocrFatGuid_t guid) __attribute__((unused));

#endif /* __OCR_GUID__H_ */
