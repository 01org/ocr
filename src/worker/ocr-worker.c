/* Copyright (c) 2012, Rice University

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:

   1.  Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
   2.  Redistributions in binary form must reproduce the above
   copyright notice, this list of conditions and the following
   disclaimer in the documentation and/or other materials provided
   with the distribution.
   3.  Neither the name of Intel Corporation
   nor the names of its contributors may be used to endorse or
   promote products derived from this software without specific
   prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include <assert.h>
#include <stdlib.h>

#include "ocr-types.h"
#include "ocr-guid.h"
#include "ocr-worker.h"

extern ocrWorker_t * newWorkerHc(ocrWorkerFactory_t * factory, void * per_type_configuration, void * per_instance_configuration);
extern ocrWorker_t * newWorkerFsimXE(ocrWorkerFactory_t * factory, void * per_type_configuration, void * per_instance_configuration);
extern ocrWorker_t * newWorkerFsimCE(ocrWorkerFactory_t * factory, void * per_type_configuration, void * per_instance_configuration);

ocrWorker_t * newWorker(ocr_worker_kind workerType, void * per_type_configuration, void * per_instance_configuration) {
    switch(workerType) {
    case OCR_WORKER_XE:
        return newWorkerFsimXE(NULL, per_type_configuration, per_instance_configuration);
    case OCR_WORKER_CE:
        return newWorkerFsimCE(NULL, per_type_configuration, per_instance_configuration);
    case OCR_WORKER_HC:
        return newWorkerHc(NULL, per_type_configuration, per_instance_configuration);
    }
    assert(false && "Unrecognized worker kind");
    return NULL;
}

ocrGuid_t getCurrentEdt() {
    ocrGuid_t workerGuid = ocr_get_current_worker_guid();
    ocrWorker_t *worker = NULL;
    globalGuidProvider->getVal(globalGuidProvider, workerGuid, (u64*)&(worker), NULL);
    return worker->getCurrentEDT(worker);
}
