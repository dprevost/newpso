/*
 * This file was generated by the program errorParser
 * using the input file quasarErrors.xml.
 * Date: Sun Sep 06 11:27:51 2009.
 *
 * The version of the XML error file is 0.4.
 *
 * Copyright (C) 2006-2008 Daniel Prevost
 *
 * This file is part of the Photon framework (photonsoftware.org).
 *
 * This file may be distributed and/or modified under the terms of the 
 * GNU General Public License version 2 or version 3 as published by 
 * the Free Software Foundation and appearing in the file COPYING.GPL2 
 * and COPYING.GPL3 included in the packaging of this software.
 *
 * Licensees holding a valid Photon Commercial license can use this 
 * file in accordance with the terms of their license.
 *
 * This software is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include <stdlib.h> /* Needed for NULL. */

#define BULDING_ERROR_MESSAGE
#include "quasarErrorHandler.h"

struct qsr_MsgStruct
{
    int  errorNumber;
    const char * message;
};

typedef struct qsr_MsgStruct qsr_MsgStruct;

/* QSR_OK */
qsr_MsgStruct qsr_Msg0 = {
    0, "No error..." };

/* QSR_NOT_ENOUGH_HEAP_MEMORY */
qsr_MsgStruct qsr_Msg1 = {
    1, "Not enough heap memory." };

/* QSR_CFG_BCK_LOCATION_TOO_LONG */
qsr_MsgStruct qsr_Msg2 = {
    2, "Backup path is too long." };

/* QSR_NO_VERIFICATION_POSSIBLE */
qsr_MsgStruct qsr_Msg3 = {
    3, "Cannot verify the shared memory." };

/* QSR_MKDIR_FAILURE */
qsr_MsgStruct qsr_Msg4 = {
    4, "Cannot create the directory for the shared memory." };

/* QSR_FILE_NOT_ACCESSIBLE */
qsr_MsgStruct qsr_Msg5 = {
    5, "Cannot access the backstore file for the shared memory." };

/* QSR_COPY_BCK_FAILURE */
qsr_MsgStruct qsr_Msg6 = {
    6, "Error while creating a copy of the backstore file for the shared memory." };

/* QSR_CREATE_BACKSTORE_FAILURE */
qsr_MsgStruct qsr_Msg7 = {
    7, "Error creating the backstore file for the shared memory." };

/* QSR_OPEN_BACKSTORE_FAILURE */
qsr_MsgStruct qsr_Msg8 = {
    8, "Error opening the backstore file for the shared memory." };

/* QSR_INCOMPATIBLE_VERSIONS */
qsr_MsgStruct qsr_Msg9 = {
    9, "Memory-file version mismatch." };

/* QSR_BACKSTORE_FILE_MISSING */
qsr_MsgStruct qsr_Msg10 = {
    10, "The shared-memory backstore file is missing." };

/* QSR_ERROR_OPENING_MEMORY */
qsr_MsgStruct qsr_Msg11 = {
    11, "Generic i/o error when attempting to open the shared memory." };

/* QSR_XML_CONFIG_NOT_OPEN */
qsr_MsgStruct qsr_Msg12 = {
    100, "Error opening the XML config file." };

/* QSR_XML_READ_ERROR */
qsr_MsgStruct qsr_Msg13 = {
    101, "Error reading the XML config file." };

/* QSR_XML_NO_ROOT */
qsr_MsgStruct qsr_Msg14 = {
    102, "No root element in the XML document." };

/* QSR_XML_INVALID_ROOT */
qsr_MsgStruct qsr_Msg15 = {
    103, "The root element is not the expected root." };

/* QSR_XML_NO_SCHEMA_LOCATION */
qsr_MsgStruct qsr_Msg16 = {
    104, "The attribute xsi:schemaLocation was not found." };

/* QSR_XML_MALFORMED_SCHEMA_LOCATION */
qsr_MsgStruct qsr_Msg17 = {
    105, "The attribute xsi:schemaLocation is not well formed." };

/* QSR_XML_PARSER_CONTEXT_FAILED */
qsr_MsgStruct qsr_Msg18 = {
    106, "The creation of a new schema parser context failed." };

/* QSR_XML_PARSE_SCHEMA_FAILED */
qsr_MsgStruct qsr_Msg19 = {
    107, "Failure when parsing the XML schema for config." };

/* QSR_XML_VALID_CONTEXT_FAILED */
qsr_MsgStruct qsr_Msg20 = {
    108, "The creation of a new XML Schemas validation context failed." };

/* QSR_XML_VALIDATION_FAILED */
qsr_MsgStruct qsr_Msg21 = {
    109, "Document validation for the config file failed." };

/* QSR_CFG_PSO_LOCATION_TOO_LONG */
qsr_MsgStruct qsr_Msg22 = {
    110, "Config: mem_location is too long." };

/* QSR_CFG_PSO_LOCATION_IS_MISSING */
qsr_MsgStruct qsr_Msg23 = {
    111, "Config: mem_location is missing." };

/* QSR_CFG_MEM_SIZE_IS_MISSING */
qsr_MsgStruct qsr_Msg24 = {
    112, "Config: mem_size is missing." };

/* QSR_CFG_SIZE_IS_MISSING */
qsr_MsgStruct qsr_Msg25 = {
    113, "Config: mem_size:size is missing." };

/* QSR_CFG_UNITS_IS_MISSING */
qsr_MsgStruct qsr_Msg26 = {
    114, "Config: mem_size:units is missing." };

/* QSR_CFG_QUASAR_ADDRESS_IS_MISSING */
qsr_MsgStruct qsr_Msg27 = {
    115, "Config: quasar_address is missing." };

/* QSR_CFG_FILE_ACCESS_IS_MISSING */
qsr_MsgStruct qsr_Msg28 = {
    116, "Config: file_access is missing." };

/* QSR_CFG_ACCESS_IS_MISSING */
qsr_MsgStruct qsr_Msg29 = {
    117, "Config: file_access:access is missing." };

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Array of pointers to previous structs */
qsr_MsgStruct * qsr_MsgArray[30] = {
    &qsr_Msg0,
    &qsr_Msg1,
    &qsr_Msg2,
    &qsr_Msg3,
    &qsr_Msg4,
    &qsr_Msg5,
    &qsr_Msg6,
    &qsr_Msg7,
    &qsr_Msg8,
    &qsr_Msg9,
    &qsr_Msg10,
    &qsr_Msg11,
    &qsr_Msg12,
    &qsr_Msg13,
    &qsr_Msg14,
    &qsr_Msg15,
    &qsr_Msg16,
    &qsr_Msg17,
    &qsr_Msg18,
    &qsr_Msg19,
    &qsr_Msg20,
    &qsr_Msg21,
    &qsr_Msg22,
    &qsr_Msg23,
    &qsr_Msg24,
    &qsr_Msg25,
    &qsr_Msg26,
    &qsr_Msg27,
    &qsr_Msg28,
    &qsr_Msg29
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

const char * qsr_ErrorMessage( int errnum )
{
    int i;

    for ( i = 0; i < 30; ++i ) {
        if ( errnum == qsr_MsgArray[i]->errorNumber ) {
            return qsr_MsgArray[i]->message;
        }
    }

    return NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

