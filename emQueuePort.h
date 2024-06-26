/*
 * emQueuePort.h
 *
 *  Created on: 23 giu 2023
 *      Author: vitomannone, Luca Catalano, Luca Eugenio Corrado
 */

#ifndef EMQUEUEPORT_H_
#define EMQUEUEPORT_H_

#include "PresentazioneEsame.h"
/*
 * Enabling this the user can allow the code to manage the copy of the data
 * structure using ElemType.h, otherwise the pointer to the data structure will be
 * returned and the user must handle it inside a critical section
 */
#define QUEUE_USE_LOCK_MECHANISM 1
#define QUEUE_USE_EMALLOC 0

/*
 * Necessary definition for lock/unlock functions
 */
#if QUEUE_USE_LOCK_MECHANISM
#include <pthread.h>
#include <semaphore.h>
#define emQueuePort_EnterCritical(ptrSem)			( !sem_wait(ptrSem) )
#define emQueuePort_ExitCritical(ptrSem)			( !sem_post(ptrSem) )
#define emQueuePort_InitBynSem(strName)				( my_sem_init(1, strName) )
#define emQueuePort_BynSemDelete(ptrSem)			( sem_close(ptrSem) )
typedef sem_t* Queue_sem_t;
#else
#define emQueuePort_EnterCritical(ptrSem)			( 1 )
#define emQueuePort_ExitCritical(ptrSem)			(void)ptrSem
#define emQueuePort_InitBynSem(strName)				( NULL ); (void)strName
#define emQueuePort_BynSemDelete(ptrSem)			(void)ptrSem
typedef void * Queue_sem_t;
#endif /* USE_LOCK_MECHANISM */
/*
 * Necessary definition for data structure handling
 */
#include "emCircularBuffer.h"
#define emQueuePort_InitStruct(nbStructures, structSize, strName)		emCircularInit(nbStructures, structSize, strName)
#define emQueueport_DeleteStruct(ptrStruct)								emCircularDelete(ptrStruct)
#define emQueuePort_StructIsFull(ptrStruct)								emCircularIsFull(ptrStruct)
#define emQueuePort_StructIsEmpty(ptrStruct)							emCircularIsEmpty(ptrStruct)
#define emQueuePort_StructGetHead(ptrStruct)							emCircularGetHead(ptrStruct)
#define emQueuePort_StructGetTail(ptrStruct)							emCircularGetTail(ptrStruct)
#define emQueuePort_Stampa(ptrStruct)                                   stampaCoda(ptrStruct)
/*
 * Necessary definition to use the policy lifespan
 */

#include "emManager.h"
#define emQueuePort_Manage(Struct)							manage(Struct)

/*
 * Necessary definition to handle the specific data type of the structure
 * The user can allow the code to manage the copy of the data
 * structure using ElemType.h
 */

#include "ElemType.h"
#define emQueuePort_ElemCpy(ptrSrc, ptrDest, size) 				elemCopy(ptrSrc, ptrDest)

/*
 * Necessary definitions for memory allocation
 */
#if QUEUE_USE_EMALLOC
#include "emAlloc.h"
#define emQueuePort_Malloc(nbBytes)								emMalloc(nbBytes)
#define emQueuePort_Free(ptr)									emFree(ptr)
#else
#define emQueuePort_Malloc(nbBytes)								malloc(nbBytes)
#define emQueuePort_Free(ptr)									free(ptr)
#endif /* EMALLOC_H_ */

#endif /* EMQUEUEPORT_H_ */
