/*
 * This file was generated by the program errorParser
 * using the input file quasarErrors.xml.
 * Date: Wed Sep 30 11:11:49 2009.
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

#ifndef QUASARERRORS_H
#define QUASARERRORS_H

#ifdef __cplusplus
extern "C" {
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum qsrErrors
{
    /**
     * No error...
     */
    QSR_OK = 0,

    /**
     * The program encountered an error while trying to allocate memory 
     * (outside of the shared memory itself). Usually a failure with 
     * malloc() or similar functions.
     */
    QSR_NOT_ENOUGH_HEAP_MEMORY = 1,

    /**
     * The path of the backup path is longer than PATH_MAX (the maximum 
     * length of a filename/pathname).
     */
    QSR_CFG_BCK_LOCATION_TOO_LONG = 2,

    /**
     * The shared memory does not exist and therefore cannot be 
     * verified.
     */
    QSR_NO_VERIFICATION_POSSIBLE = 3,

    /**
     * Cannot create the directory that will be used for the shared 
     * memory and associated files.
     */
    QSR_MKDIR_FAILURE = 4,

    /**
     * Cannot access the backstore file for the shared memory. You 
     * might want to verify the permissions and ownership of the file 
     * and/or of the parent directory.
     */
    QSR_FILE_NOT_ACCESSIBLE = 5,

    /**
     * Error while creating a copy of the backstore file for the shared 
     * memory (a backup copy is created before the shared memory is 
     * verify for inconsistencies).
     */
    QSR_COPY_BCK_FAILURE = 6,

    /**
     * Error creating the backstore file for the shared memory. 
     * Possible reasons: not enough disk space, invalid file 
     * permissions, etc.
     */
    QSR_CREATE_BACKSTORE_FAILURE = 7,

    /**
     * Error opening the backstore file for the shared memory.
     */
    QSR_OPEN_BACKSTORE_FAILURE = 8,

    /**
     * The version of the existing memory-file on disk does not match 
     * the version supported by quasar.
     */
    QSR_INCOMPATIBLE_VERSIONS = 9,

    /**
     * The shared-memory backstore file is missing. The existence of 
     * this file was already verified previously - if it is missing, 
     * something really weird is going on.
     */
    QSR_BACKSTORE_FILE_MISSING = 10,

    /**
     * Generic i/o error when attempting to open the shared memory.
     */
    QSR_ERROR_OPENING_MEMORY = 11,

    /*
     * XML config errors
     *
     * This group of errors described potential errors with the code to 
     * read and parse the XML configuration file.
     */

    /**
     * Error opening the XML configuration file.
     *
     * Most likely reason: the file does not exist but it might also be 
     * a problem with access permissions (on the file itself or one of 
     * its parent directories.
     */
    QSR_XML_CONFIG_NOT_OPEN = 100,

    /**
     * Error reading the XML configuration file.
     *
     * No validation is done at this point. Therefore the error is 
     * likely something like a missing end-tag or some other 
     * non-conformance to the XML's syntax rules.
     *
     * A simple Google search for "well-formed xml" returns many web 
     * sites that describe the syntax rules for XML. You can also use 
     * the program xmllint (included in the distribution of libxm2) to 
     * pinpoint the issue.
     */
    QSR_XML_READ_ERROR = 101,

    /**
     * No root element in the XML document.
     *
     * This error is very unlikely - without a root element, the 
     * document cannot be parsed into a tree and you'll get the error 
     * XML_READ_ERROR instead.
     */
    QSR_XML_NO_ROOT = 102,

    /**
     * The root element is not the expected root, <quasar_config>.
     */
    QSR_XML_INVALID_ROOT = 103,

    /**
     * The root element must have an attribute named schemaLocation (in 
     * the namespace "http://www.w3.org/2001/XMLSchema-instance") to 
     * point to the schema use for quasar configuration file.
     */
    QSR_XML_NO_SCHEMA_LOCATION = 104,

    /**
     * The attribute schemaLocation of the root element should contain 
     * both the namespace and the location of the schema file, 
     * separated by a space. You'll get this error if the white space 
     * is not found.
     */
    QSR_XML_MALFORMED_SCHEMA_LOCATION = 105,

    /**
     * The creation of a new schema parser context failed. There might 
     * be multiple reasons for this, for example, a memory-allocation 
     * failure in libxml2. However, the most likely reason is that the 
     * schema file is not at the location indicated by the attribute 
     * schemaLocation of the root element of the config file.
     */
    QSR_XML_PARSER_CONTEXT_FAILED = 106,

    /**
     * The parse operation of the schema failed. Most likely, there is 
     * an error in the schema for the configuration. To debug this you 
     * can use xmllint (part of the libxml2 package).
     */
    QSR_XML_PARSE_SCHEMA_FAILED = 107,

    /**
     * The creation of a new schema validation context failed. There 
     * might be multiple reasons for this, for example, a 
     * memory-allocation failure in libxml2.
     */
    QSR_XML_VALID_CONTEXT_FAILED = 108,

    /**
     * Document validation for the config file failed. To debug this 
     * problem you can use xmllint (part of the libxml2 package).
     */
    QSR_XML_VALIDATION_FAILED = 109,

    /**
     * The path associated with the tag <mem_location> is longer than 
     * PATH_MAX (the maximum length of a filename/pathname).
     */
    QSR_CFG_PSO_LOCATION_TOO_LONG = 110,

    /**
     * The tag <mem_location> is missing. This would normally be caught 
     * when the configuration is validated using the schema unless of 
     * course there is a bug in the code or the schema itself was 
     * modified.
     */
    QSR_CFG_PSO_LOCATION_IS_MISSING = 111,

    /**
     * The tag <mem_size> is missing. This would normally be caught 
     * when the configuration is validated using the schema unless of 
     * course there is a bug in the code or the schema itself was 
     * modified.
     */
    QSR_CFG_MEM_SIZE_IS_MISSING = 112,

    /**
     * The attribute "size" of the tag <mem_size> is missing. This 
     * would normally be caught when the configuration is validated 
     * using the schema unless of course there is a bug in the code or 
     * the schema itself was modified.
     */
    QSR_CFG_SIZE_IS_MISSING = 113,

    /**
     * The attribute "units" of the tag <mem_size> is missing. This 
     * would normally be caught when the configuration is validated 
     * using the schema unless of course there is a bug in the code or 
     * the schema itself was modified.
     */
    QSR_CFG_UNITS_IS_MISSING = 114,

    /**
     * The tag <quasar_address> is missing. This would normally be 
     * caught when the configuration is validated using the schema 
     * unless of course there is a bug in the code or the schema itself 
     * was modified.
     */
    QSR_CFG_QUASAR_ADDRESS_IS_MISSING = 115,

    /**
     * The tag <file_access> is missing. This would normally be caught 
     * when the configuration is validated using the schema unless of 
     * course there is a bug in the code or the schema itself was 
     * modified.
     */
    QSR_CFG_FILE_ACCESS_IS_MISSING = 116,

    /**
     * The attribute "access" of the tag <file_access> is missing. This 
     * would normally be caught when the configuration is validated 
     * using the schema unless of course there is a bug in the code or 
     * the schema itself was modified.
     */
    QSR_CFG_ACCESS_IS_MISSING = 117
};

typedef enum qsrErrors qsrErrors;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
}
#endif

#endif /* QUASARERRORS_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

