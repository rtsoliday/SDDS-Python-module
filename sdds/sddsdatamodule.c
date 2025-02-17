/**
 * @file sddsdatamodule.c
 * @brief Python C extension module (sddsdata) for interfacing with the C language SDDS library.
 * 
 * This file provides a set of Python bindings to the SDDS library for handling
 * scientific data in the Self-Describing Data Sets (SDDS) format.
 */
#include <Python.h>
#include "SDDS.h"

#if PY_MAJOR_VERSION >= 3
#define IS_PY3K
#define PyString_Check PyUnicode_Check
#define PyString_AsString PyUnicode_AsUTF8
#define PyString_FromString PyUnicode_FromString
#define PyInt_Check PyLong_Check
#define PyInt_AsLong PyLong_AsLong
#define PyInt_AsUnsignedLong PyLong_AsUnsignedLong
#define PyInt_AsLongLong PyLong_AsLongLong
#define PyInt_AsUnsignedLongLong PyLong_AsUnsignedLongLong
#define PyExc_StandardError PyExc_Exception
#endif
#if PY_MAJOR_VERSION < 3
#define PyInt_AsUnsignedLongLong PyInt_AsUnsignedLongLongMask
#define PyInt_AsUnsignedLong PyInt_AsUnsignedLongMask
#define PyInt_AsLongLong PyInt_AsLong
#endif

/** 
 * @brief Array of SDDS dataset structures.
 * 
 * This array maintains a collection of SDDS_DATASET structures
 * that correspond to files being managed by the SDDS library.
 */
SDDS_DATASET dataset_f[20];

/**
 * @brief Initializes an SDDS dataset for input from a file.
 * 
 * @param self Unused pointer to the module object.
 * @param args Python tuple containing:
 *  - fileIndex (long): Index of the dataset in the array.
 *  - filename (char*): Name of the input file.
 * 
 * @return PyObject* 
 *  - 1 on success.
 *  - 0 on error.
 */
static PyObject* sddsdata_InitializeInput( PyObject* self, PyObject* args ) 
{
  long fileIndex;
  char *filename;
  if (!PyArg_ParseTuple(args, "ls", &fileIndex, &filename)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_InitializeInput(&dataset_f[fileIndex], filename));
}

/**
 * @brief Initializes an SDDS dataset for appending data to a file.
 * 
 * @param self Unused pointer to the module object.
 * @param args Python tuple containing:
 *  - fileIndex (long): Index of the dataset in the array.
 *  - filename (char*): Name of the file to append to.
 * 
 * @return PyObject* 
 *  - 1 on success.
 *  - 0 on error.
 */
static PyObject* sddsdata_InitializeAppend( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *filename;
  if (!PyArg_ParseTuple(args, "ls", &fileIndex, &filename)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_InitializeAppend(&dataset_f[fileIndex], filename));
}

/**
 * @brief Initializes appending data to an existing SDDS page in a file.
 * 
 * @param self Unused pointer to the module object.
 * @param args Python tuple containing:
 *  - fileIndex (long): Index of the dataset in the array.
 *  - filename (char*): Name of the file to append to.
 *  - updateInterval (long): Interval at which data should be updated.
 * 
 * @return PyObject* 
 *  - Number of rows in the page on success.
 *  - 0 on error.
 */
static PyObject* sddsdata_InitializeAppendToPage( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *filename;
  long updateInterval, result;
  int64_t rowsPresent;
  if (!PyArg_ParseTuple(args, "lsl", &fileIndex, &filename, &updateInterval)) {
    return 0;
  }
  result = SDDS_InitializeAppendToPage(&dataset_f[fileIndex], filename, updateInterval, &rowsPresent);
  if (result != 1)
    return 0;
  return PyLong_FromLongLong((long long)rowsPresent);
}

/**
 * @brief Initializes an SDDS dataset for output to a file.
 * 
 * @param self Unused pointer to the module object.
 * @param args Python tuple containing:
 *  - fileIndex (long): Index of the dataset in the array.
 *  - data_mode (long): Data mode (e.g., SDDS_BINARY or SDDS_ASCII).
 *  - lines_per_row (long): Number of lines per row in the output file.
 *  - description (char*): Description of the dataset.
 *  - contents (char*): Contents of the dataset.
 *  - filename (char*): Name of the output file.
 * 
 * @return PyObject* 
 *  - 1 on success.
 *  - 0 on error.
 */
static PyObject* sddsdata_InitializeOutput( PyObject* self, PyObject* args )
{
  long fileIndex;
  long data_mode;
  long lines_per_row;
  char *description;
  char *contents;
  char *filename;
  if (!PyArg_ParseTuple(args, "lllsss", &fileIndex, &data_mode, &lines_per_row, &description, &contents, &filename)) {
    return 0;
  }
  if (description) 
    if (strlen(description) == 0) 
      description = NULL;
  if (contents) 
    if (strlen(contents) == 0)
      contents = NULL;
  return PyLong_FromLong(SDDS_InitializeOutput(&dataset_f[fileIndex], data_mode, lines_per_row, description, contents, filename));
}

/**
 * @brief Sets the data order for the SDDS dataset to column-major.
 * 
 * @param self Unused pointer to the module object.
 * @param args Python tuple containing:
 *  - fileIndex (long): Index of the dataset in the array.
 * 
 * @return PyObject* 
 *  - Py_None (NULL return value in Python).
 */
static PyObject* sddsdata_SetColumnMajorOrder( PyObject* self, PyObject* args )
{
  long fileIndex;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return Py_None;
  }
  dataset_f[fileIndex].layout.data_mode.column_major = 1;
  return Py_None;
}

/**
 * @brief Sets the row count mode for an SDDS dataset to fixed row count mode.
 * 
 * @param self Unused pointer to the module object.
 * @param args Python tuple containing:
 *  - fileIndex (long): Index of the dataset in the array.
 * 
 * @return PyObject* 
 *  - Py_None (Python's None).
 */
static PyObject* sddsdata_SetFixedRowCountMode( PyObject* self, PyObject* args )
{
  long fileIndex;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return Py_None;
  }
  SDDS_SetRowCountMode(&dataset_f[fileIndex], SDDS_FIXEDROWCOUNT);
  return Py_None;
}

/**
 * @brief Sets the data order for the SDDS dataset to row-major.
 * 
 * @param self Unused pointer to the module object.
 * @param args Python tuple containing:
 *  - fileIndex (long): Index of the dataset in the array.
 * 
 * @return PyObject* 
 *  - Py_None (Python's None).
 */
static PyObject* sddsdata_SetRowMajorOrder( PyObject* self, PyObject* args )
{
  long fileIndex;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return Py_None;
  }
  dataset_f[fileIndex].layout.data_mode.column_major = 0;
  return Py_None;
}

/**
 * @brief Enables filesystem synchronization (fsync) for an SDDS dataset.
 * 
 * @param self Unused pointer to the module object.
 * @param args Python tuple containing:
 *  - fileIndex (long): Index of the dataset in the array.
 * 
 * @return PyObject* 
 *  - Py_None (Python's None).
 */
static PyObject* sddsdata_EnableFSync( PyObject* self, PyObject* args )
{
  long fileIndex;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return Py_None;
  }
  dataset_f[fileIndex].layout.data_mode.fsync_data = 1;
  return Py_None;
}

/**
 * @brief Disables filesystem synchronization (fsync) for an SDDS dataset.
 * 
 * @param self Unused pointer to the module object.
 * @param args Python tuple containing:
 *  - fileIndex (long): Index of the dataset in the array.
 * 
 * @return PyObject* 
 *  - Py_None (Python's None).
 */
static PyObject* sddsdata_DisableFSync( PyObject* self, PyObject* args )
{
  long fileIndex;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return Py_None;
  }
  dataset_f[fileIndex].layout.data_mode.fsync_data = 0;
  return Py_None;
}

/**
 * @brief Terminates an SDDS dataset, releasing any associated resources.
 * 
 * @param self Unused pointer to the module object.
 * @param args Python tuple containing:
 *  - fileIndex (long): Index of the dataset in the array.
 * 
 * @return PyObject* 
 *  - 1 on success.
 *  - 0 on error.
 */
static PyObject* sddsdata_Terminate( PyObject* self, PyObject* args )
{
  long fileIndex;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_Terminate(&dataset_f[fileIndex]));
}

/**
 * @brief Sets the termination mode for SDDS to avoid freeing strings in arrays and tables.
 * 
 * Sets the termination mode to `TERMINATE_DONT_FREE_ARRAY_STRINGS` and 
 * `TERMINATE_DONT_FREE_TABLE_STRINGS`.
 * 
 * @param self Unused pointer to the module object.
 * @param args Unused.
 * 
 * @return PyObject* 
 *  - Py_None (Python's None).
 */
static PyObject* sddsdata_SetTerminateMode( PyObject* self, PyObject* args )
{
  SDDS_SetTerminateMode(TERMINATE_DONT_FREE_TABLE_STRINGS+TERMINATE_DONT_FREE_ARRAY_STRINGS);
  Py_INCREF(Py_None);
  return Py_None;
}

/**
 * @brief Defines a parameter in an SDDS dataset.
 * 
 * @param self Unused pointer to the module object.
 * @param args Python tuple containing:
 *  - fileIndex (long): Index of the dataset in the array.
 *  - name (char*): Name of the parameter.
 *  - symbol (char*): Symbol for the parameter (optional).
 *  - units (char*): Units of the parameter (optional).
 *  - description (char*): Description of the parameter (optional).
 *  - format_string (char*): Format string for displaying the parameter (optional).
 *  - type (long): Data type of the parameter (e.g., SDDS_LONG, SDDS_DOUBLE).
 *  - fixed_value (PyObject*): Fixed value for the parameter.
 * 
 * @return PyObject* 
 *  - Index of the defined parameter on success.
 *  - -1 on error.
 */
static PyObject* sddsdata_DefineParameter( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *name;
  char *symbol;
  char *units;
  char *description;
  char *format_string;
  long type;
  PyObject *fixed_value;
  char *fixedStringValue;
  int32_t fixedLongValue;
  double fixedDoubleValue;
  char buffer[SDDS_MAXLINE];
  if (!PyArg_ParseTuple(args, "lssssslO", &fileIndex, &name , &symbol , &units , &description , &format_string , &type , &fixed_value)) {
    return PyLong_FromLong(-1);
  }
  if (symbol)
    if (strlen(symbol) == 0) 
      symbol = NULL;
  if (units)
    if (strlen(units) == 0)
      units = NULL;
  if (description)
    if (strlen(description) == 0)
      description = NULL;
  if (format_string)
    if (strlen(format_string) == 0)
      format_string = NULL;

  if (PyString_Check(fixed_value)) {
    fixedStringValue = (char*)PyString_AsString(fixed_value);
    if (fixedStringValue)
      if (strlen(fixedStringValue) == 0)
        fixedStringValue = NULL;
    return PyLong_FromLong(SDDS_DefineParameter(&dataset_f[fileIndex], 
						name, symbol, units, 
						description, format_string, 
						type, fixedStringValue));
  } else {
    if (PyNumber_Check(fixed_value)) {
      if (PyLong_Check(fixed_value)) {
	fixedLongValue = (int32_t)PyLong_AsLong(fixed_value);
	SDDS_SprintTypedValue(&fixedLongValue, 0, SDDS_LONG , format_string, buffer, 0);
	return PyLong_FromLong(SDDS_DefineParameter(&dataset_f[fileIndex], 
						    name, symbol, units, 
						    description, format_string, 
						    type, buffer));
      } else if (PyInt_Check(fixed_value)) {
	fixedLongValue = (int32_t)PyInt_AsLong(fixed_value);
	SDDS_SprintTypedValue(&fixedLongValue, 0, SDDS_LONG , format_string, buffer, 0);
	return PyLong_FromLong(SDDS_DefineParameter(&dataset_f[fileIndex], 
						    name, symbol, units, 
						    description, format_string, 
						    type, buffer));
      } else if (PyFloat_Check(fixed_value)) {
	fixedDoubleValue = PyFloat_AsDouble(fixed_value);
	SDDS_SprintTypedValue(&fixedDoubleValue, 0, SDDS_DOUBLE , format_string, buffer, 0);
	return PyLong_FromLong(SDDS_DefineParameter(&dataset_f[fileIndex], 
						    name, symbol, units, 
						    description, format_string, 
						    type, buffer));
      }
    }
    return PyLong_FromLong(SDDS_DefineParameter1(&dataset_f[fileIndex], 
						   name, symbol, units, 
						   description, format_string, 
						   type, NULL));
  }
}

/**
 * @brief Defines an array in an SDDS dataset.
 *
 * @param self Unused.
 * @param args A tuple containing:
 *             - fileIndex (long): Index of the dataset.
 *             - name (char*): Name of the array.
 *             - symbol (char*): Symbol for the array.
 *             - units (char*): Units of the array.
 *             - description (char*): Description of the array.
 *             - format_string (char*): Format string for the array.
 *             - type (long): Data type of the array.
 *             - field_length (long): Length of the fields in the array.
 *             - dimensions (long): Number of dimensions in the array.
 *             - group_name (char*): Name of the group to which the array belongs.
 * 
 * @return PyObject* 
 *  - Index of the defined array on success.
 *  - -1 on error.
 */
static PyObject* sddsdata_DefineArray( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *name;
  char *symbol;
  char *units;
  char *description;
  char *format_string;
  long type;
  long field_length;
  long dimensions;
  char *group_name;
  if (!PyArg_ParseTuple(args, "lsssssslll", &fileIndex, &name , &symbol, &units, &description, &format_string, &group_name, &type, &field_length, &dimensions)) {
    return PyLong_FromLong(-1);
  }
  if (symbol) 
    if (strlen(symbol) == 0)
      symbol = NULL;
  if (units) 
    if (strlen(units) == 0)
      units = NULL;
  if (description) 
    if (strlen(description) == 0)
      description = NULL;   
  if (format_string) 
    if (strlen(format_string) == 0)
      format_string = NULL;
  if (group_name) 
    if (strlen(group_name) == 0)
      group_name = NULL;
  return PyLong_FromLong(SDDS_DefineArray(&dataset_f[fileIndex], name, symbol, units, description, format_string, type, field_length, dimensions, group_name));
}

/**
 * @brief Defines a column in an SDDS dataset.
 *
 * @param self Unused.
 * @param args A tuple containing:
 *             - fileIndex (long): Index of the dataset.
 *             - name (char*): Name of the column.
 *             - symbol (char*): Symbol for the column.
 *             - units (char*): Units of the column.
 *             - description (char*): Description of the column.
 *             - format_string (char*): Format string for the column.
 *             - type (long): Data type of the column.
 *             - field_length (long): Length of the fields in the column.
 * 
 * @return PyObject* 
 *  - Index of the defined column on success.
 *  - -1 on error.
 */
static PyObject* sddsdata_DefineColumn( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *name;
  char *symbol;
  char *units;
  char *description;
  char *format_string;
  long type;
  long field_length;
  if (!PyArg_ParseTuple(args, "lsssssll", &fileIndex, &name , &symbol, &units, &description, &format_string, &type, &field_length)) {
    return PyLong_FromLong(-1);
  }
  if (symbol) 
    if (strlen(symbol) == 0)
      symbol = NULL;
  if (units) 
    if (strlen(units) == 0)
      units = NULL;
  if (description) 
    if (strlen(description) == 0)
      description = NULL;   
  if (format_string) 
    if (strlen(format_string) == 0)
      format_string = NULL;
  return PyLong_FromLong(SDDS_DefineColumn(&dataset_f[fileIndex], name, symbol, units, description, format_string, type, field_length));
}

/**
 * @brief Validates a name for SDDS compatibility.
 *
 * @param self Unused.
 * @param args A tuple containing:
 *             - name (char*): The name to validate.
 * 
 * @return PyObject* A Python integer (1 if valid, 0 if invalid).
 */
static PyObject* sddsdata_IsValidName( PyObject* self, PyObject* args )
{
  char *name;
  if (!PyArg_ParseTuple(args, "s", &name)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_IsValidName(name, NULL));
}

/**
 * @brief Sets name validity flags to allow any name.
 *
 * @param self Unused.
 * @param args Unused.
 * 
 * @return PyObject* A Python integer representing the previous value of nameValidityFlags.
 */
static PyObject* sddsdata_SetNameValidityFlags( PyObject* self, PyObject* args )
{
  return PyLong_FromLong(SDDS_SetNameValidityFlags(SDDS_ALLOW_ANY_NAME));
}

/**
 * @brief Defines a simple column in an SDDS dataset.
 *
 * @param self Unused.
 * @param args A tuple containing:
 *             - fileIndex (long): Index of the dataset.
 *             - name (char*): Name of the column.
 *             - units (char*): Units of the column.
 *             - type (long): Data type of the column.
 * 
 * @return PyObject* A Python integer (1 on success, 0 on error).
 */
static PyObject* sddsdata_DefineSimpleColumn( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *name;
  char *units;
  long type;
  if (!PyArg_ParseTuple(args, "lssl", &fileIndex, &name, &units, &type)) {
    return 0;
  }
  if (units) 
    if (strlen(units) == 0)
      units = NULL;   
  return PyLong_FromLong(SDDS_DefineSimpleColumn(&dataset_f[fileIndex], name, units, type));
}

/**
 * @brief Defines a simple array in an SDDS dataset.
 *
 * @param self Unused.
 * @param args A tuple containing:
 *             - fileIndex (long): Index of the dataset.
 *             - name (char*): Name of the array.
 *             - units (char*): Units of the array.
 *             - type (long): Data type of the array.
 *             - dimensions (long): Number of dimensions in the array.
 * 
 * @return PyObject* A Python integer (1 on success, 0 on error).
 */
static PyObject* sddsdata_DefineSimpleArray( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *name;
  char *units;
  long type;
  long dimensions;
  long results;
  if (!PyArg_ParseTuple(args, "lssll", &fileIndex, &name, &units, &type, &dimensions)) {
    return 0;
  }
  if (units) 
    if (strlen(units) == 0)
      units = NULL;
  results = SDDS_DefineArray(&dataset_f[fileIndex], name, NULL, units, NULL, NULL, type, 0, dimensions, NULL);
  if (results == -1) {
    results = 0;
  } else {
    results = 1;
  }
  return PyLong_FromLong(results);
}

/**
 * @brief Defines a simple parameter in an SDDS dataset.
 *
 * @param self Unused.
 * @param args A tuple containing:
 *             - fileIndex (long): Index of the dataset.
 *             - name (char*): Name of the parameter.
 *             - units (char*): Units of the parameter.
 *             - type (long): Data type of the parameter.
 * 
 * @return PyObject* A Python integer (1 on success, 0 on error).
 */
static PyObject* sddsdata_DefineSimpleParameter( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *name;
  char *units;
  long type;
  if (!PyArg_ParseTuple(args, "lssl", &fileIndex, &name, &units, &type)) {
    return 0;
  }
  if (units) 
    if (strlen(units) == 0)
      units = NULL;   
  return PyLong_FromLong(SDDS_DefineSimpleParameter(&dataset_f[fileIndex], name, units, type));
}

/**
 * @brief Writes the layout of an SDDS dataset.
 *
 * @param self Unused.
 * @param args A tuple containing:
 *             - fileIndex (long): Index of the dataset.
 * 
 * @return PyObject* A Python integer (1 on success, 0 on error).
 */
static PyObject* sddsdata_WriteLayout( PyObject* self, PyObject* args )
{
  long fileIndex;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_WriteLayout(&dataset_f[fileIndex]));
}

/**
 * @brief Erases data from an SDDS dataset.
 *
 * @param self Unused.
 * @param args A tuple containing:
 *             - fileIndex (long): Index of the dataset.
 * 
 * @return PyObject* A Python integer (1 on success, 0 on error).
 */
static PyObject* sddsdata_EraseData( PyObject* self, PyObject* args )
{
  long fileIndex;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_EraseData(&dataset_f[fileIndex]));
}


/**
 * @brief Processes a column definition string.
 *
 * @param self Unused.
 * @param args A tuple containing:
 *             - fileIndex (long): Index of the dataset.
 *             - string (char*): Column definition string.
 *             - mode (long): Mode for processing (0 for SDDS_NORMAL_DEFINITION, 1 for SDDS_WRITEONLY_DEFINITION).
 *
 * @return PyObject* A Python integer (1 on success, 0 on error).
 */
static PyObject* sddsdata_ProcessColumnString( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *string;
  long mode;
  if (!PyArg_ParseTuple(args, "lsl", &fileIndex, &string, &mode)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_ProcessColumnString(&dataset_f[fileIndex], string, mode));
}

/**
 * @brief Processes an array definition string.
 *
 * @param self Unused.
 * @param args A tuple containing:
 *             - fileIndex (long): Index of the dataset.
 *             - string (char*): Array definition string.
 *
 * @return PyObject* A Python integer (1 on success, 0 on error).
 */
static PyObject* sddsdata_ProcessArrayString( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *string;
  if (!PyArg_ParseTuple(args, "ls", &fileIndex, &string)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_ProcessArrayString(&dataset_f[fileIndex], string));
}

/**
 * @brief Processes a parameter definition string.
 *
 * @param self Unused.
 * @param args A tuple containing:
 *             - fileIndex (long): Index of the dataset.
 *             - string (char*): Parameter definition string.
 *             - mode (long): Mode for processing (0 for SDDS_NORMAL_DEFINITION, 1 for SDDS_WRITEONLY_DEFINITION).
 *
 * @return PyObject* A Python integer (1 on success, 0 on error).
 */
static PyObject* sddsdata_ProcessParameterString( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *string;
  long mode;
  if (!PyArg_ParseTuple(args, "lsl", &fileIndex, &string, &mode)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_ProcessParameterString(&dataset_f[fileIndex], string, mode));
}

/**
 * @brief Initializes a copy from one SDDS dataset to another.
 *
 * @param self Unused.
 * @param args A tuple containing:
 *             - fileIndexTarget (long): Index of the target dataset.
 *             - fileIndexSource (long): Index of the source dataset.
 *             - filename (char*): Name of the target file (can be NULL).
 *             - filemode (char*): File mode for the target file ("r", "w", "rb", "wb", "m").
 *
 * @return PyObject* A Python integer (1 on success, 0 on error).
 */
static PyObject* sddsdata_InitializeCopy( PyObject* self, PyObject* args )
{
  long fileIndexTarget;
  long fileIndexSource;
  char *filename;
  char *filemode;
  if (!PyArg_ParseTuple(args, "llss", &fileIndexTarget, &fileIndexSource, &filename, &filemode)) {
    return 0;
  }
  if (filename)
    if (strlen(filename) == 0) 
      filename = NULL;
  return PyLong_FromLong(SDDS_InitializeCopy(&dataset_f[fileIndexTarget], &dataset_f[fileIndexSource], filename, filemode));
}

/**
 * @brief Copies the layout from one SDDS dataset to another.
 *
 * @param self Unused.
 * @param args A tuple containing:
 *             - fileIndexTarget (long): Index of the target dataset.
 *             - fileIndexSource (long): Index of the source dataset.
 *
 * @return PyObject* A Python integer (1 on success, 0 on error).
 */
static PyObject* sddsdata_CopyLayout( PyObject* self, PyObject* args )
{
  long fileIndexTarget;
  long fileIndexSource;
  if (!PyArg_ParseTuple(args, "ll", &fileIndexTarget, &fileIndexSource)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_CopyLayout(&dataset_f[fileIndexTarget], &dataset_f[fileIndexSource]));
}

/**
 * @brief Appends the layout of one SDDS dataset to another.
 *
 * @param self Unused.
 * @param args A tuple containing:
 *             - fileIndexTarget (long): Index of the target dataset.
 *             - fileIndexSource (long): Index of the source dataset.
 *
 * @return PyObject* A Python integer (1 on success, 0 on error).
 */
static PyObject* sddsdata_AppendLayout( PyObject* self, PyObject* args )
{
  long fileIndexTarget;
  long fileIndexSource;
  if (!PyArg_ParseTuple(args, "ll", &fileIndexTarget, &fileIndexSource)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_AppendLayout(&dataset_f[fileIndexTarget], &dataset_f[fileIndexSource], 0));
}

/**
 * @brief Copies a page from one SDDS dataset to another.
 *
 * @param self Unused.
 * @param args A tuple containing:
 *             - fileIndexTarget (long): Index of the target dataset.
 *             - fileIndexSource (long): Index of the source dataset.
 *
 * @return PyObject* A Python integer (1 on success, 0 on error).
 */
static PyObject* sddsdata_CopyPage( PyObject* self, PyObject* args )
{
  long fileIndexTarget;
  long fileIndexSource;
  if (!PyArg_ParseTuple(args, "ll", &fileIndexTarget, &fileIndexSource)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_CopyPage(&dataset_f[fileIndexTarget], &dataset_f[fileIndexSource]));
}

/**
 * @brief Copies parameters from one SDDS dataset to another.
 *
 * @param self Unused.
 * @param args A tuple containing:
 *             - fileIndexTarget (long): Index of the target dataset.
 *             - fileIndexSource (long): Index of the source dataset.
 *
 * @return PyObject* A Python integer (1 on success, 0 on error).
 */
static PyObject* sddsdata_CopyParameters( PyObject* self, PyObject* args )
{
  long fileIndexTarget;
  long fileIndexSource;
  if (!PyArg_ParseTuple(args, "ll", &fileIndexTarget, &fileIndexSource)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_CopyParameters(&dataset_f[fileIndexTarget], &dataset_f[fileIndexSource]));
}

/**
 * @brief Copies arrays from one SDDS dataset to another.
 *
 * @param self Unused.
 * @param args A tuple containing:
 *             - fileIndexTarget (long): Index of the target dataset.
 *             - fileIndexSource (long): Index of the source dataset.
 *
 * @return PyObject* A Python integer (1 on success, 0 on error).
 */
static PyObject* sddsdata_CopyArrays( PyObject* self, PyObject* args )
{
  long fileIndexTarget;
  long fileIndexSource;
  if (!PyArg_ParseTuple(args, "ll", &fileIndexTarget, &fileIndexSource)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_CopyArrays(&dataset_f[fileIndexTarget], &dataset_f[fileIndexSource]));
}

/**
 * @brief Copies columns from one SDDS dataset to another.
 *
 * @param self Unused.
 * @param args A tuple containing:
 *             - fileIndexTarget (long): Index of the target dataset.
 *             - fileIndexSource (long): Index of the source dataset.
 *
 * @return PyObject* A Python integer (1 on success, 0 on error).
 */
static PyObject* sddsdata_CopyColumns( PyObject* self, PyObject* args )
{
  long fileIndexTarget;
  long fileIndexSource;
  if (!PyArg_ParseTuple(args, "ll", &fileIndexTarget, &fileIndexSource)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_CopyColumns(&dataset_f[fileIndexTarget], &dataset_f[fileIndexSource]));
}

/**
 * @brief Copies a row from one SDDS dataset to another.
 *
 * @param self Unused.
 * @param args A tuple containing:
 *             - fileIndexTarget (long): Index of the target dataset.
 *             - target_row (long): Row number in the target dataset.
 *             - fileIndexSource (long): Index of the source dataset.
 *             - source_row (long): Row number in the source dataset.
 *
 * @return PyObject* A Python integer (1 on success, 0 on error).
 */
static PyObject* sddsdata_CopyRow( PyObject* self, PyObject* args )
{
  long fileIndexTarget;
  long target_row;
  long fileIndexSource;
  long source_row;
  if (!PyArg_ParseTuple(args, "llll", &fileIndexTarget, &target_row, &fileIndexSource, &source_row)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_CopyRow(&dataset_f[fileIndexTarget], target_row, &dataset_f[fileIndexSource], source_row));
}

/**
 * @brief Directly copies a row from one SDDS dataset to another.
 *
 * @param self Unused.
 * @param args A tuple containing:
 *             - fileIndexTarget (long): Index of the target dataset.
 *             - target_row (long): Row number in the target dataset.
 *             - fileIndexSource (long): Index of the source dataset.
 *             - source_row (long): Row number in the source dataset.
 *
 * @return PyObject* A Python integer (1 on success, 0 on error).
 */
static PyObject* sddsdata_CopyRowDirect( PyObject* self, PyObject* args )
{
  long fileIndexTarget;
  long target_row;
  long fileIndexSource;
  long source_row;
  if (!PyArg_ParseTuple(args, "llll", &fileIndexTarget, &target_row, &fileIndexSource, &source_row)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_CopyRowDirect(&dataset_f[fileIndexTarget], target_row, &dataset_f[fileIndexSource], source_row));
}

/**
 * @brief Copies additional rows from one SDDS dataset to another.
 *
 * @param self Unused.
 * @param args A tuple containing:
 *             - fileIndexTarget (long): Index of the target dataset.
 *             - fileIndexSource (long): Index of the source dataset.
 *
 * @return PyObject* A Python integer (1 on success, 0 on error).
 */
static PyObject* sddsdata_CopyAdditionalRows( PyObject* self, PyObject* args )
{
  long fileIndexTarget;
  long fileIndexSource;
  if (!PyArg_ParseTuple(args, "ll", &fileIndexTarget, &fileIndexSource)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_CopyAdditionalRows(&dataset_f[fileIndexTarget], &dataset_f[fileIndexSource]));
}

/**
 * @brief Defers saving the layout of an SDDS dataset.
 *
 * @param self Unused.
 * @param args A tuple containing:
 *             - fileIndex (long): Index of the dataset.
 *             - mode (long): Mode for deferring (0 for False, 1 for True).
 *
 * @return PyObject* None.
 */
static PyObject* sddsdata_DeferSavingLayout( PyObject* self, PyObject* args )
{
  long fileIndex, mode;
  if (!PyArg_ParseTuple(args, "ll", &fileIndex, &mode)) {
    return Py_None;
  }
  SDDS_DeferSavingLayout(&dataset_f[fileIndex],mode);
  Py_INCREF(Py_None);
  return Py_None;
}

/**
 * @brief Saves the layout of an SDDS dataset.
 *
 * @param self Unused.
 * @param args A tuple containing:
 *             - fileIndex (long): Index of the dataset.
 *
 * @return PyObject* A Python integer (1 on success, 0 on error).
 */
static PyObject* sddsdata_SaveLayout( PyObject* self, PyObject* args )
{
  long fileIndex;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_SaveLayout(&dataset_f[fileIndex]));
}

/**
 * @brief Restores the layout of an SDDS dataset.
 *
 * @param self Unused.
 * @param args A tuple containing:
 *             - fileIndex (long): Index of the dataset.
 *
 * @return PyObject* A Python integer (1 on success, 0 on error).
 */
static PyObject* sddsdata_RestoreLayout( PyObject* self, PyObject* args )
{
  long fileIndex;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_RestoreLayout(&dataset_f[fileIndex]));
}

/**
 * @brief Starts a page in an SDDS dataset.
 *
 * @param self Unused.
 * @param args A tuple containing:
 *             - fileIndex (long): Index of the dataset.
 *             - expected_n_rows (long): Expected number of rows on the page.
 *
 * @return PyObject* A Python integer (1 on success, 0 on error).
 */
static PyObject* sddsdata_StartPage( PyObject* self, PyObject* args )
{
  long fileIndex;
  long expected_n_rows;
  if (!PyArg_ParseTuple(args, "ll", &fileIndex, &expected_n_rows)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_StartPage(&dataset_f[fileIndex], expected_n_rows));
}

/**
 * @brief Clears the current page in an SDDS dataset.
 *
 * @param self Unused.
 * @param args A tuple containing:
 *             - fileIndex (long): Index of the dataset.
 *
 * @return PyObject* A Python integer (1 on success, 0 on error).
 */
static PyObject* sddsdata_ClearPage( PyObject* self, PyObject* args )
{
  long fileIndex;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_ClearPage(&dataset_f[fileIndex]));
}

/**
 * @brief Lengthens the table by adding additional rows.
 *
 * @param self Pointer to the Python self object.
 * @param args Tuple containing:
 *   - fileIndex (long): Index of the file dataset.
 *   - n_additional_rows (long): Number of rows to add.
 * @return PyObject*:
 *   - 0 on error.
 *   - 1 on success.
 */
static PyObject* sddsdata_LengthenTable( PyObject* self, PyObject* args )
{
  long fileIndex;
  long n_additional_rows;
  if (!PyArg_ParseTuple(args, "ll", &fileIndex, &n_additional_rows)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_LengthenTable(&dataset_f[fileIndex], n_additional_rows));
}

/**
 * @brief Writes the current page of the dataset to the file.
 *
 * @param self Pointer to the Python self object.
 * @param args Tuple containing:
 *   - fileIndex (long): Index of the file dataset.
 * @return PyObject*:
 *   - 0 on error.
 *   - 1 on success.
 */
static PyObject* sddsdata_WritePage( PyObject* self, PyObject* args )
{
  long fileIndex;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_WritePage(&dataset_f[fileIndex]));
}

/**
 * @brief Updates the page based on the specified mode.
 *
 * @param self Pointer to the Python self object.
 * @param args Tuple containing:
 *   - fileIndex (long): Index of the file dataset.
 *   - mode (long): Update mode.
 * @return PyObject*:
 *   - 0 on error.
 *   - 1 on success.
 */
static PyObject* sddsdata_UpdatePage( PyObject* self, PyObject* args )
{
  long fileIndex, mode;
  if (!PyArg_ParseTuple(args, "ll", &fileIndex, &mode)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_UpdatePage(&dataset_f[fileIndex], mode));
}

/**
 * @brief Initializes a headerless input for a dataset file.
 *
 * @param self Pointer to the Python self object.
 * @param args Tuple containing:
 *   - fileIndex (long): Index of the file dataset.
 *   - filename (char*): Name of the file to initialize.
 * @return PyObject*:
 *   - 0 on error.
 *   - 1 on success.
 */
static PyObject* sddsdata_InitHeaderlessInput( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *filename;
  if (!PyArg_ParseTuple(args, "ls", &fileIndex, &filename)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_InitializeHeaderlessInput(&dataset_f[fileIndex], filename));
}

/**
 * @brief Reads the current page from the dataset file.
 *
 * @param self Pointer to the Python self object.
 * @param args Tuple containing:
 *   - fileIndex (long): Index of the file dataset.
 * @return PyObject*:
 *   - 0 on error.
 *   - -1 on EOF.
 *   - Page number on success.
 */
static PyObject* sddsdata_ReadPage( PyObject* self, PyObject* args )
{
  long fileIndex;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_ReadPage(&dataset_f[fileIndex]));
}

/**
 * @brief Reads a sparse page from the dataset file.
 *
 * @param self Pointer to the Python self object.
 * @param args Tuple containing:
 *   - fileIndex (long): Index of the file dataset.
 *   - sparse_interval (long): Interval for sparse reading.
 *   - sparse_offset (long): Offset for sparse reading.
 * @return PyObject*:
 *   - 0 on error.
 *   - -1 on EOF.
 *   - Page number on success.
 */
static PyObject* sddsdata_ReadPageSparse( PyObject* self, PyObject* args )
{
  long fileIndex;
  long sparse_interval;
  long sparse_offset;
  if (!PyArg_ParseTuple(args, "lll", &fileIndex, &sparse_interval, &sparse_offset)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_ReadPageSparse(&dataset_f[fileIndex], 0, sparse_interval, sparse_offset, 0));
}

/**
 * @brief Reads the last rows of the current page from the dataset file.
 *
 * @param self Pointer to the Python self object.
 * @param args Tuple containing:
 *   - fileIndex (long): Index of the file dataset.
 *   - last_rows (long): Number of last rows to read.
 * @return PyObject*:
 *   - 0 on error.
 *   - -1 on EOF.
 *   - Page number on success.
 */
static PyObject* sddsdata_ReadPageLastRows( PyObject* self, PyObject* args )
{
  long fileIndex;
  long last_rows;
  if (!PyArg_ParseTuple(args, "ll", &fileIndex, &last_rows)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_ReadPageLastRows(&dataset_f[fileIndex], last_rows));
}

/**
 * @brief Gets the number of rows in the dataset.
 *
 * @param self Pointer to the Python self object.
 * @param args Tuple containing:
 *   - fileIndex (long): Index of the file dataset.
 * @return PyObject*:
 *   - Number of rows in the dataset.
 */
static PyObject* sddsdata_RowCount( PyObject* self, PyObject* args )
{
  long fileIndex;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return 0;
  }
  return PyLong_FromLongLong((long long)SDDS_RowCount(&dataset_f[fileIndex]));
}

/**
 * @brief Sets the column flags for the dataset.
 *
 * @param self Pointer to the Python self object.
 * @param args Tuple containing:
 *   - fileIndex (long): Index of the file dataset.
 *   - column_flag_value (long): Flag value (valid values: 0, 1).
 * @return PyObject*:
 *   - 0 on error.
 *   - 1 on success.
 */
static PyObject* sddsdata_SetColumnFlags( PyObject* self, PyObject* args )
{
  long fileIndex;
  long column_flag_value;
  if (!PyArg_ParseTuple(args, "ll", &fileIndex, &column_flag_value)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_SetColumnFlags(&dataset_f[fileIndex], column_flag_value));
}

/**
 * @brief Sets the row flags for the dataset.
 *
 * @param self Pointer to the Python self object.
 * @param args Tuple containing:
 *   - fileIndex (long): Index of the file dataset.
 *   - row_flag_value (long): Flag value (valid values: 0, 1).
 * @return PyObject*:
 *   - 0 on error.
 *   - 1 on success.
 */
static PyObject* sddsdata_SetRowFlags( PyObject* self, PyObject* args )
{
  long fileIndex;
  long row_flag_value;
  if (!PyArg_ParseTuple(args, "ll", &fileIndex, &row_flag_value)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_SetRowFlags(&dataset_f[fileIndex], row_flag_value));
}

/**
 * @brief Gets the flag value for a specific row in the dataset.
 *
 * @param self Pointer to the Python self object.
 * @param args Tuple containing:
 *   - fileIndex (long): Index of the file dataset.
 *   - row (long): Row number for which the flag is retrieved.
 * @return PyObject*:
 *   - -1 on error.
 *   - Row flag on success.
 */
static PyObject* sddsdata_GetRowFlag( PyObject* self, PyObject* args )
{
  long fileIndex;
  long row;
  if (!PyArg_ParseTuple(args, "ll", &fileIndex, &row)) {
    return PyLong_FromLong(-1);
  }
  return PyLong_FromLong(SDDS_GetRowFlag(&dataset_f[fileIndex], row));
}

/**
 * @brief Deletes a column from the dataset.
 * @param self Python object (unused).
 * @param args Python arguments tuple containing:
 *  - fileIndex (long): Index of the dataset file.
 *  - column_name (char*): Name of the column to delete.
 * @return PyObject*:
 *  - PyLong(0) on error.
 *  - PyLong(1) on success.
 */
static PyObject* sddsdata_DeleteColumn( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *column_name;
  if (!PyArg_ParseTuple(args, "ls", &fileIndex, &column_name)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_DeleteColumn(&dataset_f[fileIndex], column_name));
}

/**
 * @brief Deletes a parameter from the dataset.
 * @param self Python object (unused).
 * @param args Python arguments tuple containing:
 *  - fileIndex (long): Index of the dataset file.
 *  - parameter_name (char*): Name of the parameter to delete.
 * @return PyObject*:
 *  - PyLong(0) on error.
 *  - PyLong(1) on success.
 */
static PyObject* sddsdata_DeleteParameter( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *parameter_name;
  if (!PyArg_ParseTuple(args, "ls", &fileIndex, &parameter_name)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_DeleteParameter(&dataset_f[fileIndex], parameter_name));
}

/**
 * @brief Deletes all unset columns in the dataset.
 * @param self Python object (unused).
 * @param args Python arguments tuple containing:
 *  - fileIndex (long): Index of the dataset file.
 * @return PyObject*:
 *  - PyLong(0) on error.
 *  - PyLong(1) on success.
 */
static PyObject* sddsdata_DeleteUnsetColumns( PyObject* self, PyObject* args )
{
  long fileIndex;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_DeleteUnsetColumns(&dataset_f[fileIndex]));
}

/**
 * @brief Deletes all unset rows in the dataset.
 * @param self Python object (unused).
 * @param args Python arguments tuple containing:
 *  - fileIndex (long): Index of the dataset file.
 * @return PyObject*:
 *  - PyLong(0) on error.
 *  - PyLong(1) on success.
 */
static PyObject* sddsdata_DeleteUnsetRows( PyObject* self, PyObject* args )
{
  long fileIndex;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_DeleteUnsetRows(&dataset_f[fileIndex]));
}

/**
 * @brief Gets the count of columns in the dataset.
 * @param self Python object (unused).
 * @param args Python arguments tuple containing:
 *  - fileIndex (long): Index of the dataset file.
 * @return PyObject*: Number of columns as PyLong.
 */
static PyObject* sddsdata_ColumnCount( PyObject* self, PyObject* args )
{
  long fileIndex;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_ColumnCount(&dataset_f[fileIndex]));
}

/**
 * @brief Gets the count of arrays in the dataset.
 * @param self Python object (unused).
 * @param args Python arguments tuple containing:
 *  - fileIndex (long): Index of the dataset file.
 * @return PyObject*: Number of arrays as PyLong.
 */
static PyObject* sddsdata_ArrayCount( PyObject* self, PyObject* args )
{
  long fileIndex;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_ArrayCount(&dataset_f[fileIndex]));
}

/**
 * @brief Gets the count of parameters in the dataset.
 * @param self Python object (unused).
 * @param args Python arguments tuple containing:
 *  - fileIndex (long): Index of the dataset file.
 * @return PyObject*: Number of parameters as PyLong.
 */
static PyObject* sddsdata_ParameterCount( PyObject* self, PyObject* args )
{
  long fileIndex;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_ParameterCount(&dataset_f[fileIndex]));
}

/**
 * @brief Gets the description (text and contents) of the dataset.
 * @param self Python object (unused).
 * @param args Python arguments tuple containing:
 *  - fileIndex (long): Index of the dataset file.
 * @return PyObject*:
 *  - PyList of [text, contents] on success.
 *  - NULL on error.
 */
static PyObject* sddsdata_GetDescription( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *text;
  char *contents;
  PyObject *v;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return NULL;
  }
  if (SDDS_GetDescription(&dataset_f[fileIndex], &text, &contents) == 0) {
    return NULL;
  }
  if (!(v = PyList_New(2)))
    return NULL;
  if (text) {
    PyList_SetItem(v, 0, PyString_FromString(text));
    free(text);
  } else {
    PyList_SetItem(v, 0, PyString_FromString(""));
  }
  if (contents) {
    PyList_SetItem(v, 1, PyString_FromString(contents));
    free(contents);
  } else {
    PyList_SetItem(v, 1, PyString_FromString(""));
  }
  return v;
}

/**
 * @brief Retrieves the description text for a dataset.
 * 
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *             - `fileIndex` (long): Index of the dataset file.
 * @return Python string containing the description text, or NULL on error.
 */
static PyObject* sddsdata_GetDescriptionText( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *text;
  char *contents;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return NULL;
  }
  if (SDDS_GetDescription(&dataset_f[fileIndex], &text, &contents) == 0) {
    return NULL;
  }
  if (text)
    return PyString_FromString(text);
  return PyString_FromString("");
}

/**
 * @brief Retrieves the description contents for a dataset.
 * 
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *             - `fileIndex` (long): Index of the dataset file.
 * @return Python string containing the description contents, or NULL on error.
 */
static PyObject* sddsdata_GetDescriptionContents( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *text;
  char *contents;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return NULL;
  }
  if (SDDS_GetDescription(&dataset_f[fileIndex], &text, &contents) == 0) {
    return NULL;
  }
  if (contents)
    return PyString_FromString(contents);
  return PyString_FromString("");
}

/**
 * @brief Returns the number of errors recorded.
 * 
 * @param self Python object (unused).
 * @param args Python tuple (unused).
 * @return Python integer representing the number of errors.
 */
static PyObject* sddsdata_NumberOfErrors( PyObject* self, PyObject* args ) 
{
  return PyLong_FromLong(SDDS_NumberOfErrors());
}

/**
 * @brief Clears all recorded errors.
 * 
 * @param self Python object (unused).
 * @param args Python tuple (unused).
 * @return Python None.
 */
static PyObject* sddsdata_ClearErrors( PyObject* self, PyObject* args ) 
{
  SDDS_ClearErrors();
  Py_INCREF(Py_None);
  return Py_None;
}

/**
 * @brief Sets an error description.
 * 
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *             - `error_text` (char*): Description of the error.
 * @return Python None, or NULL on error.
 */
static PyObject* sddsdata_SetError( PyObject* self, PyObject* args ) 
{
  char *error_text;
  if (!PyArg_ParseTuple(args, "s", &error_text)) {
    return NULL;
  }
  SDDS_SetError(error_text);
  Py_INCREF(Py_None);
  return Py_None;
}

/**
 * @brief Displays a message on stderr and exits the program.
 * 
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *             - `message` (char*): Message to display.
 * @return Python None, or NULL on error.
 */
static PyObject* sddsdata_Bomb( PyObject* self, PyObject* args ) 
{
  char *message;
  if (!PyArg_ParseTuple(args, "s", &message)) {
    return NULL;
  }
  SDDS_Bomb(message);
  Py_INCREF(Py_None);
  return Py_None;
}

/**
 * @brief Displays a warning message on stderr.
 * 
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *             - `message` (char*): Warning message to display.
 * @return Python None, or NULL on error.
 */
static PyObject* sddsdata_Warning( PyObject* self, PyObject* args ) 
{
  char *message;
  if (!PyArg_ParseTuple(args, "s", &message)) {
    return NULL;
  }
  SDDS_Warning(message);
  Py_INCREF(Py_None);
  return Py_None;
}

/**
 * @brief Registers a program name for use in error messages.
 * 
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *             - `name` (char*): Name of the program.
 * @return Python None, or NULL on error.
 */
static PyObject* sddsdata_RegisterProgramName( PyObject* self, PyObject* args ) 
{
  char *name;
  if (!PyArg_ParseTuple(args, "s", &name)) {
    return NULL;
  }
  SDDS_RegisterProgramName(name);
  Py_INCREF(Py_None);
  return Py_None;
}

/**
 * @brief Prints error messages based on the provided mode.
 * 
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *             - `mode` (long): Error print mode. Valid values: 0, 1, 2, 3.
 * @return Python None, or NULL on error.
 */
static PyObject* sddsdata_PrintErrors( PyObject* self, PyObject* args ) 
{
  long mode;
  if (!PyArg_ParseTuple(args, "l", &mode)) {
    return NULL;
  }
  SDDS_PrintErrors(stderr, mode);
  Py_INCREF(Py_None);
  return Py_None;
}

/**
 * @brief Transfers a column definition from one dataset to another.
 * 
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *             - `fileIndexTarget` (long): Target dataset index.
 *             - `fileIndexSource` (long): Source dataset index.
 *             - `name` (char*): Name of the column to transfer.
 *             - `newName` (char*): New name for the column (optional).
 * @return Python integer (1 on success, 0 on error).
 */
static PyObject* sddsdata_TransferColumnDefinition( PyObject* self, PyObject* args )
{
  long fileIndexTarget;
  long fileIndexSource;
  char *name;
  char *newName;
  if (!PyArg_ParseTuple(args, "llss", &fileIndexTarget, &fileIndexSource, &name, &newName)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_TransferColumnDefinition(&dataset_f[fileIndexTarget], &dataset_f[fileIndexSource], name, newName));
}

/**
 * @brief Transfers an array definition from one dataset to another.
 * 
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *             - `fileIndexTarget` (long): Target dataset index.
 *             - `fileIndexSource` (long): Source dataset index.
 *             - `name` (char*): Name of the array to transfer.
 *             - `newName` (char*): New name for the array (optional).
 * @return Python integer (1 on success, 0 on error).
 */
static PyObject* sddsdata_TransferArrayDefinition( PyObject* self, PyObject* args )
{
  long fileIndexTarget;
  long fileIndexSource;
  char *name;
  char *newName;
  if (!PyArg_ParseTuple(args, "llss", &fileIndexTarget, &fileIndexSource, &name, &newName)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_TransferArrayDefinition(&dataset_f[fileIndexTarget], &dataset_f[fileIndexSource], name, newName));
}

/**
 * @brief Transfers a parameter definition from one dataset to another.
 * 
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *             - `fileIndexTarget` (long): Target dataset index.
 *             - `fileIndexSource` (long): Source dataset index.
 *             - `name` (char*): Name of the parameter to transfer.
 *             - `newName` (char*): New name for the parameter (optional).
 * @return Python integer (1 on success, 0 on error).
 */
static PyObject* sddsdata_TransferParameterDefinition( PyObject* self, PyObject* args )
{
  long fileIndexTarget;
  long fileIndexSource;
  char *name;
  char *newName;
  if (!PyArg_ParseTuple(args, "llss", &fileIndexTarget, &fileIndexSource, &name, &newName)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_TransferParameterDefinition(&dataset_f[fileIndexTarget], &dataset_f[fileIndexSource], name, newName));
}

/**
 * @brief Defines a column in one dataset based on a parameter in another dataset.
 * 
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *             - `fileIndexTarget` (long): Target dataset index.
 *             - `fileIndexSource` (long): Source dataset index.
 *             - `name` (char*): Name of the parameter to base the column on.
 *             - `newName` (char*): New name for the column (optional).
 * @return Python integer (1 on success, 0 on error).
 */
static PyObject* sddsdata_DefineColumnLikeParameter( PyObject* self, PyObject* args )
{
  long fileIndexTarget;
  long fileIndexSource;
  char *name;
  char *newName;
  if (!PyArg_ParseTuple(args, "llss", &fileIndexTarget, &fileIndexSource, &name, &newName)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_DefineColumnLikeParameter(&dataset_f[fileIndexTarget], &dataset_f[fileIndexSource], name, newName));
}

/**
 * @brief Defines a parameter in one dataset based on a column in another dataset.
 * 
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *             - `fileIndexTarget` (long): Target dataset index.
 *             - `fileIndexSource` (long): Source dataset index.
 *             - `name` (char*): Name of the column to base the parameter on.
 *             - `newName` (char*): New name for the parameter (optional).
 * @return Python integer (1 on success, 0 on error).
 */
static PyObject* sddsdata_DefineParameterLikeColumn( PyObject* self, PyObject* args )
{
  long fileIndexTarget;
  long fileIndexSource;
  char *name;
  char *newName;
  if (!PyArg_ParseTuple(args, "llss", &fileIndexTarget, &fileIndexSource, &name, &newName)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_DefineParameterLikeColumn(&dataset_f[fileIndexTarget], &dataset_f[fileIndexSource], name, newName));
}

/**
 * @brief Transfers all column definitions from one dataset to another.
 * 
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *             - `fileIndexTarget` (long): Target dataset index.
 *             - `fileIndexSource` (long): Source dataset index.
 *             - `mode` (long): Transfer mode. Valid values: 0, 1, 2, 3.
 * @return Python integer (1 on success, 0 on error).
 */
static PyObject* sddsdata_TransferAllColumnDefinitions( PyObject* self, PyObject* args )
{
  long fileIndexTarget;
  long fileIndexSource;
  long mode;
  if (!PyArg_ParseTuple(args, "lll", &fileIndexTarget, &fileIndexSource, &mode)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_TransferAllColumnDefinitions(&dataset_f[fileIndexTarget], &dataset_f[fileIndexSource], mode));
}

/**
 * @brief Transfers all array definitions from one dataset to another.
 * 
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *             - `fileIndexTarget` (long): Target dataset index.
 *             - `fileIndexSource` (long): Source dataset index.
 *             - `mode` (long): Transfer mode. Valid values: 0, 1, 2, 3.
 * @return Python integer (1 on success, 0 on error).
 */
static PyObject* sddsdata_TransferAllArrayDefinitions( PyObject* self, PyObject* args )
{
  long fileIndexTarget;
  long fileIndexSource;
  long mode;
  if (!PyArg_ParseTuple(args, "lll", &fileIndexTarget, &fileIndexSource, &mode)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_TransferAllArrayDefinitions(&dataset_f[fileIndexTarget], &dataset_f[fileIndexSource], mode));
}

/**
 * @brief Transfers all parameter definitions from one dataset to another.
 * 
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *             - `fileIndexTarget` (long): Target dataset index.
 *             - `fileIndexSource` (long): Source dataset index.
 *             - `mode` (long): Transfer mode. Valid values: 0, 1, 2, 3.
 * @return Python integer (1 on success, 0 on error).
 */
static PyObject* sddsdata_TransferAllParameterDefinitions( PyObject* self, PyObject* args )
{
  long fileIndexTarget;
  long fileIndexSource;
  long mode;
  if (!PyArg_ParseTuple(args, "lll", &fileIndexTarget, &fileIndexSource, &mode)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_TransferAllParameterDefinitions(&dataset_f[fileIndexTarget], &dataset_f[fileIndexSource], mode));
}

/**
 * @brief Retrieves the index of a column in a dataset.
 * 
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *             - `fileIndex` (long): Dataset index.
 *             - `name` (char*): Name of the column.
 * @return Python integer (index on success, -1 on error).
 */
static PyObject* sddsdata_GetColumnIndex( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *name;
  if (!PyArg_ParseTuple(args, "ls", &fileIndex, &name)) {
    return PyLong_FromLong(-1);
  }
  return PyLong_FromLong(SDDS_GetColumnIndex(&dataset_f[fileIndex], name));
}

/**
 * @brief Retrieves the index of an array in a dataset.
 * 
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *             - `fileIndex` (long): Dataset index.
 *             - `name` (char*): Name of the array.
 * @return Python integer (index on success, -1 on error).
 */
static PyObject* sddsdata_GetArrayIndex( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *name;
  if (!PyArg_ParseTuple(args, "ls", &fileIndex, &name)) {
    return PyLong_FromLong(-1);
  }
  return PyLong_FromLong(SDDS_GetArrayIndex(&dataset_f[fileIndex], name));
}

/**
 * @brief Retrieves the index of a parameter in a dataset.
 * 
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *             - `fileIndex` (long): Dataset index.
 *             - `name` (char*): Name of the parameter.
 * @return Python integer (index on success, -1 on error).
 */
static PyObject* sddsdata_GetParameterIndex( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *name;
  if (!PyArg_ParseTuple(args, "ls", &fileIndex, &name)) {
    return PyLong_FromLong(-1);
  }
  return PyLong_FromLong(SDDS_GetParameterIndex(&dataset_f[fileIndex], name));
}

/**
 * @brief Retrieves the type of a column in a dataset.
 * 
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *             - `fileIndex` (long): Dataset index.
 *             - `index` (long): Column index.
 * @return Python integer (type on success, 0 on error).
 */
static PyObject* sddsdata_GetColumnType( PyObject* self, PyObject* args )
{
  long fileIndex;
  long index;
  if (!PyArg_ParseTuple(args, "ll", &fileIndex, &index)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_GetColumnType(&dataset_f[fileIndex], index));
}

/**
 * @brief Retrieves the type of an array in a dataset.
 * 
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *             - `fileIndex` (long): Dataset index.
 *             - `index` (long): Array index.
 * @return Python integer (type on success, 0 on error).
 */
static PyObject* sddsdata_GetArrayType( PyObject* self, PyObject* args )
{
  long fileIndex;
  long index;
  if (!PyArg_ParseTuple(args, "ll", &fileIndex, &index)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_GetArrayType(&dataset_f[fileIndex], index));
}

/**
 * @brief Retrieves the type of a column by name in a dataset.
 * 
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *             - `fileIndex` (long): Dataset index.
 *             - `name` (char*): Name of the column.
 * @return Python integer (type on success, 0 on error).
 */
static PyObject* sddsdata_GetNamedColumnType( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *name;
  if (!PyArg_ParseTuple(args, "ls", &fileIndex, &name)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_GetNamedColumnType(&dataset_f[fileIndex], name));
}

/**
 * @brief Retrieves the type of an array by name in a dataset.
 * 
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *             - `fileIndex` (long): Dataset index.
 *             - `name` (char*): Name of the array.
 * @return Python integer (type on success, 0 on error).
 */
static PyObject* sddsdata_GetNamedArrayType( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *name;
  if (!PyArg_ParseTuple(args, "ls", &fileIndex, &name)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_GetNamedArrayType(&dataset_f[fileIndex], name));
}

/**
 * @brief Retrieves the definition of a column in a dataset.
 * 
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *             - `fileIndex` (long): Dataset index.
 *             - `name` (char*): Name of the column.
 * @return Python list containing column attributes on success, NULL on error.
 */
static PyObject* sddsdata_GetColumnDefinition( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *name;
  COLUMN_DEFINITION *pardef;
  PyObject *v;
  if (!PyArg_ParseTuple(args, "ls", &fileIndex, &name)) {
    return NULL;
  }
  if ((pardef = SDDS_GetColumnDefinition(&dataset_f[fileIndex], name)) == NULL)
    return NULL;
  if (!(v = PyList_New(6)))
    return NULL;

  if (pardef->symbol)
    PyList_SetItem(v, 0, PyString_FromString(pardef->symbol));
  else
    PyList_SetItem(v, 0, PyString_FromString(""));
  if (pardef->units)
    PyList_SetItem(v, 1, PyString_FromString(pardef->units));
  else
    PyList_SetItem(v, 1, PyString_FromString(""));
  if (pardef->description)
    PyList_SetItem(v, 2, PyString_FromString(pardef->description));
  else
    PyList_SetItem(v, 2, PyString_FromString(""));
  if (pardef->format_string)
    PyList_SetItem(v, 3, PyString_FromString(pardef->format_string));
  else
    PyList_SetItem(v, 3, PyString_FromString(""));
  PyList_SetItem(v, 4, PyLong_FromLong(pardef->type));
  PyList_SetItem(v, 5, PyLong_FromLong(pardef->field_length));

  SDDS_FreeColumnDefinition(pardef);

  return v;
}

/**
 * @brief Retrieves the definition of an array in a dataset.
 * 
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *             - `fileIndex` (long): Dataset index.
 *             - `name` (char*): Name of the array.
 * @return Python list containing array attributes on success, NULL on error.
 */
static PyObject* sddsdata_GetArrayDefinition( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *name;
  ARRAY_DEFINITION *pardef;
  PyObject *v;
  if (!PyArg_ParseTuple(args, "ls", &fileIndex, &name)) {
    return NULL;
  }
  if ((pardef = SDDS_GetArrayDefinition(&dataset_f[fileIndex], name)) == NULL)
    return NULL;
  if (!(v = PyList_New(8)))
    return NULL;

  if (pardef->symbol)
    PyList_SetItem(v, 0, PyString_FromString(pardef->symbol));
  else
    PyList_SetItem(v, 0, PyString_FromString(""));
  if (pardef->units)
    PyList_SetItem(v, 1, PyString_FromString(pardef->units));
  else
    PyList_SetItem(v, 1, PyString_FromString(""));
  if (pardef->description)
    PyList_SetItem(v, 2, PyString_FromString(pardef->description));
  else
    PyList_SetItem(v, 2, PyString_FromString(""));
  if (pardef->format_string)
    PyList_SetItem(v, 3, PyString_FromString(pardef->format_string));
  else
    PyList_SetItem(v, 3, PyString_FromString(""));
  if (pardef->group_name)
    PyList_SetItem(v, 4, PyString_FromString(pardef->group_name));
  else
    PyList_SetItem(v, 4, PyString_FromString(""));
  PyList_SetItem(v, 5, PyLong_FromLong(pardef->type));
  PyList_SetItem(v, 6, PyLong_FromLong(pardef->field_length));
  PyList_SetItem(v, 7, PyLong_FromLong(pardef->dimensions));

  SDDS_FreeArrayDefinition(pardef);

  return v;
}

/**
 * @brief Retrieves the type of a parameter in a dataset.
 * 
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *             - `fileIndex` (long): Dataset index.
 *             - `index` (long): Parameter index.
 * @return Python integer (type on success, 0 on error).
 */
static PyObject* sddsdata_GetParameterType( PyObject* self, PyObject* args )
{
  long fileIndex;
  long index;
  if (!PyArg_ParseTuple(args, "ll", &fileIndex, &index)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_GetParameterType(&dataset_f[fileIndex], index));
}

/**
 * @brief Retrieves the type of a parameter by name in a dataset.
 * 
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *             - `fileIndex` (long): Dataset index.
 *             - `name` (char*): Name of the parameter.
 * @return Python integer (type on success, 0 on error).
 */
static PyObject* sddsdata_GetNamedParameterType( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *name;
  if (!PyArg_ParseTuple(args, "ls", &fileIndex, &name)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_GetNamedParameterType(&dataset_f[fileIndex], name));  
}

/**
 * @brief Retrieves the definition of a parameter in a dataset.
 * 
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *             - `fileIndex` (long): Dataset index.
 *             - `name` (char*): Name of the parameter.
 * @return Python list containing parameter attributes on success, NULL on error.
 */
static PyObject* sddsdata_GetParameterDefinition( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *name;
  PARAMETER_DEFINITION *pardef;
  PyObject *v;
  if (!PyArg_ParseTuple(args, "ls", &fileIndex, &name)) {
    return NULL;
  }
  if ((pardef = SDDS_GetParameterDefinition(&dataset_f[fileIndex], name)) == NULL)
    return NULL;
  if (!(v = PyList_New(6)))
    return NULL;

  if (pardef->symbol)
    PyList_SetItem(v, 0, PyString_FromString(pardef->symbol));
  else
    PyList_SetItem(v, 0, PyString_FromString(""));
  if (pardef->units)
    PyList_SetItem(v, 1, PyString_FromString(pardef->units));
  else
    PyList_SetItem(v, 1, PyString_FromString(""));
  if (pardef->description)
    PyList_SetItem(v, 2, PyString_FromString(pardef->description));
  else
    PyList_SetItem(v, 2, PyString_FromString(""));
  if (pardef->format_string)
    PyList_SetItem(v, 3, PyString_FromString(pardef->format_string));
  else
    PyList_SetItem(v, 3, PyString_FromString(""));
  PyList_SetItem(v, 4, PyLong_FromLong(pardef->type));
  if (pardef->fixed_value)
    PyList_SetItem(v, 5, PyString_FromString(pardef->fixed_value));
  else
    PyList_SetItem(v, 5, PyString_FromString(""));

  SDDS_FreeParameterDefinition(pardef);

  return v;
}

/**
 * @brief Retrieves the size of a given SDDS data type.
 * 
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *             - `type` (long): SDDS data type.
 * @return Python integer (size on success, -1 on error).
 */
static PyObject* sddsdata_GetTypeSize( PyObject* self, PyObject* args )
{
  long type;
  if (!PyArg_ParseTuple(args, "l", &type)) {
    return PyLong_FromLong(-1);
  }
  return PyLong_FromLong(SDDS_GetTypeSize(type));
}

/**
 * @brief Retrieves the name of a given SDDS data type.
 * 
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *             - `type` (long): SDDS data type.
 * @return Python string (type name on success, NULL on error).
 */
static PyObject* sddsdata_GetTypeName( PyObject* self, PyObject* args )
{
  long type;
  char *name;
  char msgbuf[256];
  if (!PyArg_ParseTuple(args, "l", &type)) {
    return NULL;
  }
  name = SDDS_GetTypeName(type);
  if (name)
    return PyString_FromString(name);
  sprintf(msgbuf, "sdds.GetTypeName: %ld is an invalid SDDS data type", type);
  PyErr_SetString(PyExc_StandardError, msgbuf);
  return NULL;
}

/**
 * @brief Identifies the SDDS data type for a given type name.
 * 
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *             - `typeName` (char*): Name of the data type (e.g., double, float).
 * @return Python integer (type on success, 0 on error).
 */
static PyObject* sddsdata_IdentifyType( PyObject* self, PyObject* args )
{
  char *typeName;
  if (!PyArg_ParseTuple(args, "s", &typeName)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_IdentifyType(typeName));
}

/**
 * @brief Verifies the existence and type of a column in a dataset.
 * 
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *             - `fileIndex` (long): Dataset index.
 *             - `name` (char*): Column name.
 *             - `units` (char*): Expected units.
 *             - `type` (long): Expected type.
 * @return Python integer indicating the check status.
 */
static PyObject* sddsdata_CheckColumn( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *name;
  char *units;
  long type;
  if (!PyArg_ParseTuple(args, "lssl", &fileIndex, &name, &units, &type)) {
    return PyLong_FromLong(SDDS_CHECK_WRONGTYPE);
  }
  return PyLong_FromLong(SDDS_CheckColumn(&dataset_f[fileIndex], name, units, type, stderr));
}

/**
 * @brief Verifies the existence and type of an array in a dataset.
 * 
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *             - `fileIndex` (long): Dataset index.
 *             - `name` (char*): Array name.
 *             - `units` (char*): Expected units.
 *             - `type` (long): Expected type.
 * @return Python integer indicating the check status.
 */
static PyObject* sddsdata_CheckArray( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *name;
  char *units;
  long type;
  if (!PyArg_ParseTuple(args, "lssl", &fileIndex, &name, &units, &type)) {
    return PyLong_FromLong(SDDS_CHECK_WRONGTYPE);
  }
  return PyLong_FromLong(SDDS_CheckArray(&dataset_f[fileIndex], name, units, type, stderr));
}

/**
 * @brief Verifies the existence and type of a parameter in a dataset.
 * 
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *             - `fileIndex` (long): Dataset index.
 *             - `name` (char*): Parameter name.
 *             - `units` (char*): Expected units.
 *             - `type` (long): Expected type.
 * @return Python integer indicating the check status.
 */
static PyObject* sddsdata_CheckParameter( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *name;
  char *units;
  long type;
  if (!PyArg_ParseTuple(args, "lssl", &fileIndex, &name, &units, &type)) {
    return PyLong_FromLong(SDDS_CHECK_WRONGTYPE);
  }
  return PyLong_FromLong(SDDS_CheckParameter(&dataset_f[fileIndex], name, units, type, stderr));
}

/**
 * @brief Determines if a string contains whitespace.
 * 
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *             - `string` (char*): String to check.
 * @return Python integer (1 if true, 0 otherwise).
 */
static PyObject* sddsdata_HasWhitespace( PyObject* self, PyObject* args )
{
  char *string;
  if (!PyArg_ParseTuple(args, "s", &string)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_HasWhitespace(string));
}

/**
 * @brief Checks if a string is blank.
 * 
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *             - `s` (char*): String to check.
 * @return Python integer (1 if true, 0 otherwise).
 */
static PyObject* sddsdata_StringIsBlank( PyObject* self, PyObject* args )
{
  char *s;
  if (!PyArg_ParseTuple(args, "s", &s)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_StringIsBlank(s));
}

/**
 * @brief Applies a scaling factor to a parameter in a dataset.
 * 
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *             - `fileIndex` (long): Dataset index.
 *             - `name` (char*): Parameter name.
 *             - `factor` (double): Scaling factor.
 * @return Python integer (1 on success, 0 on error).
 */
static PyObject* sddsdata_ApplyFactorToParameter( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *name;
  double factor;
  if (!PyArg_ParseTuple(args, "lsd", &fileIndex, &name, &factor)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_ApplyFactorToParameter(&dataset_f[fileIndex], name, factor));
}

/**
 * @brief Applies a scaling factor to a column in a dataset.
 * 
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *             - `fileIndex` (long): Dataset index.
 *             - `name` (char*): Column name.
 *             - `factor` (double): Scaling factor.
 * @return Python integer (1 on success, 0 on error).
 */
static PyObject* sddsdata_ApplyFactorToColumn( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *name;
  double factor;
  if (!PyArg_ParseTuple(args, "lsd", &fileIndex, &name, &factor)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_ApplyFactorToColumn(&dataset_f[fileIndex], name, factor));
}

/**
 * @brief Deletes fixed parameter values for a given dataset.
 *
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *        - fileIndex: Index of the dataset file.
 * 
 * @return PyObject*:
 *         - 0 on error.
 *         - 1 on success.
 */
static PyObject* sddsdata_DeleteParameterFixedValues( PyObject* self, PyObject* args )
{
  long fileIndex;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_DeleteParameterFixedValues(&dataset_f[fileIndex]));
}

/**
 * @brief Sets the data mode for a given dataset.
 *
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *        - fileIndex: Index of the dataset file.
 *        - newmode: New data mode to be set.
 * 
 * @return PyObject*:
 *         - 0 on error.
 *         - 1 on success.
 */
static PyObject* sddsdata_SetDataMode( PyObject* self, PyObject* args )
{
  long fileIndex;
  long newmode;
  if (!PyArg_ParseTuple(args, "ll", &fileIndex, &newmode)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_SetDataMode(&dataset_f[fileIndex], newmode));
}

/**
 * @brief Checks the validity of a dataset.
 *
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *        - fileIndex: Index of the dataset file.
 *        - caller: Caller information string.
 * 
 * @return PyObject*:
 *         - 0 on error.
 *         - 1 on success.
 */
static PyObject* sddsdata_CheckDataset( PyObject* self, PyObject* args )
{
  long fileIndex;
  char *caller;
  if (!PyArg_ParseTuple(args, "ls", &fileIndex, &caller)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_CheckDataset(&dataset_f[fileIndex], caller));
}

/**
 * @brief Sets the auto-check mode for datasets.
 *
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *        - newMode: New auto-check mode (0 or 1).
 * 
 * @return PyObject*: Previous mode.
 */
static PyObject* sddsdata_SetAutoCheckMode( PyObject* self, PyObject* args )
{
  long newMode;
  if (!PyArg_ParseTuple(args, "l", &newMode)) {
    return 0;
  }
  return PyLong_FromLong(SDDS_SetAutoCheckMode(newMode));
}

/**
 * @brief Retrieves a column name by its index from a dataset.
 *
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *        - fileIndex: Index of the dataset file.
 *        - index: Index of the column.
 * 
 * @return PyObject*:
 *         - NULL on error.
 *         - Column name string on success.
 */
static PyObject* sddsdata_GetColumnNameFromIndex( PyObject* self, PyObject* args )
{
  long fileIndex;
  long index;
  char **data;
  int32_t number;
  PyObject *v;
  long i;
  if (!PyArg_ParseTuple(args, "ll", &fileIndex, &index)) {
    return NULL;
  }
  data = SDDS_GetColumnNames(&dataset_f[fileIndex], &number);
  if (!(data))
    return NULL;
  if ((index >= 0) && (index < number)) {
    v = PyString_FromString(data[index]);
    for (i=0;i<number;i++)
      free(data[i]);
    free(data);
    return v;
  }
  for (i=0;i<number;i++)
    free(data[i]);
  free(data);
  return NULL;
}

/**
 * @brief Retrieves all column names from a dataset.
 *
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *        - fileIndex: Index of the dataset file.
 * 
 * @return PyObject*:
 *         - NULL on error.
 *         - List of column names on success.
 */
static PyObject* sddsdata_GetColumnNames( PyObject* self, PyObject* args )
{
  long fileIndex;
  long index;
  char **data;
  int32_t number;
  PyObject *v;
  long i;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return NULL;
  }
  data = SDDS_GetColumnNames(&dataset_f[fileIndex], &number);
  if (!(data))
    return NULL;
  if (!(v = PyList_New(number)))
    return NULL;
  for (index = 0; index < number; index++)
    PyList_SetItem(v, index, PyString_FromString(data[index]));
  for (i=0;i<number;i++)
    free(data[i]);
  free(data);
  return v;
}

/**
 * @brief Retrieves an array name by its index from a dataset.
 *
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *        - fileIndex: Index of the dataset file.
 *        - index: Index of the array.
 * 
 * @return PyObject*:
 *         - NULL on error.
 *         - Array name string on success.
 */
static PyObject* sddsdata_GetArrayNameFromIndex( PyObject* self, PyObject* args )
{
  long fileIndex;
  long index;
  char **data;
  int32_t number;
  PyObject *v;
  long i;
  if (!PyArg_ParseTuple(args, "ll", &fileIndex, &index)) {
    return NULL;
  }
  data = SDDS_GetArrayNames(&dataset_f[fileIndex], &number);
  if (!(data))
    return NULL;
  if ((index >= 0) && (index < number)) {
    v = PyString_FromString(data[index]);
    for (i=0;i<number;i++)
      free(data[i]);
    free(data);
    return v;
  }
  for (i=0;i<number;i++)
    free(data[i]);
  free(data);
  return NULL;
}

/**
 * @brief Retrieves all array names from a dataset.
 *
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *        - fileIndex: Index of the dataset file.
 * 
 * @return PyObject*:
 *         - NULL on error.
 *         - List of array names on success.
 */
static PyObject* sddsdata_GetArrayNames( PyObject* self, PyObject* args )
{
  long fileIndex;
  long index;
  char **data;
  int32_t number;
  PyObject *v;
  long i;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return NULL;
  }
  data = SDDS_GetArrayNames(&dataset_f[fileIndex], &number);
  if (!(data))
    return NULL;
  if (!(v = PyList_New(number)))
    return NULL;
  for (index = 0; index < number; index++)
    PyList_SetItem(v, index, PyString_FromString(data[index]));
  for (i=0;i<number;i++)
    free(data[i]);
  free(data);
  return v;
}

/**
 * @brief Retrieves a parameter name by its index from a dataset.
 *
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *        - fileIndex: Index of the dataset file.
 *        - index: Index of the parameter.
 * 
 * @return PyObject*:
 *         - NULL on error.
 *         - Parameter name string on success.
 */
static PyObject* sddsdata_GetParameterNameFromIndex( PyObject* self, PyObject* args )
{
  long fileIndex;
  long index;
  char **data;
  int32_t number;
  PyObject *v;
  long i;
  if (!PyArg_ParseTuple(args, "ll", &fileIndex, &index)) {
    return NULL;
  }
  data = SDDS_GetParameterNames(&dataset_f[fileIndex], &number);
  if (!(data))
    return NULL;
  if ((index >= 0) && (index < number)) {
    v = PyString_FromString(data[index]);
    for (i=0;i<number;i++)
      free(data[i]);
    free(data);
    return v;
  }
  for (i=0;i<number;i++)
    free(data[i]);
  free(data);
  return NULL;
}

/**
 * @brief Retrieves all parameter names from a dataset.
 *
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *        - fileIndex: Index of the dataset file.
 * 
 * @return PyObject*:
 *         - NULL on error.
 *         - List of parameter names on success.
 */
static PyObject* sddsdata_GetParameterNames( PyObject* self, PyObject* args )
{
  long fileIndex;
  long index;
  char **data;
  int32_t number;
  PyObject *v;
  long i;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return NULL;
  }
  data = SDDS_GetParameterNames(&dataset_f[fileIndex], &number);
  if (!(data))
    return NULL;
  if (!(v = PyList_New(number)))
    return NULL;
  for (index = 0; index < number; index++)
    PyList_SetItem(v, index, PyString_FromString(data[index]));
  for (i=0;i<number;i++)
    free(data[i]);
  free(data);
  return v;
}

/**
 * @brief Sets a parameter value in a dataset by index or name.
 *
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *        - fileIndex: Index of the dataset file.
 *        - indexOrName: Index or name of the parameter.
 *        - v: Value to set.
 * 
 * @return PyObject*:
 *         - 0 on error.
 *         - 1 on success.
 */
static PyObject* sddsdata_SetParameter( PyObject* self, PyObject* args )
{
  long fileIndex;
  PyObject *indexOrName;
  PyObject *v;
  long index;
  long type;
  if (!PyArg_ParseTuple(args, "lOO", &fileIndex, &indexOrName, &v)) {
    return 0;
  }
  if (PyString_Check(indexOrName)) {
    index = SDDS_GetParameterIndex(&dataset_f[fileIndex], (char*)PyString_AsString(indexOrName));
  } else if (PyNumber_Check(indexOrName)) {
    if (PyInt_Check(indexOrName))
      index = PyInt_AsLong(indexOrName);
    else if (PyLong_Check(indexOrName))
      index = PyLong_AsLong(indexOrName);
    else
      return 0;
  } else
    return 0;
  if ((type = SDDS_GetParameterType(&dataset_f[fileIndex], index)) == 0)
    return 0;
  switch (type) {
  case SDDS_SHORT:
  case SDDS_USHORT:
  case SDDS_LONG:
    if (PyLong_Check(v)) {
      return PyLong_FromLong(SDDS_SetParameters(&dataset_f[fileIndex],SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,index,PyLong_AsLong(v),-1));
    } else {
      return PyLong_FromLong(SDDS_SetParameters(&dataset_f[fileIndex],SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,index,PyInt_AsLong(v),-1));
    }
  case SDDS_ULONG:
    if (PyLong_Check(v)) {
      return PyLong_FromLong(SDDS_SetParameters(&dataset_f[fileIndex],SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,index,PyLong_AsUnsignedLong(v),-1));
    } else {
      return PyLong_FromLong(SDDS_SetParameters(&dataset_f[fileIndex],SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,index,PyInt_AsUnsignedLong(v),-1));
    }
  case SDDS_LONG64:
    if (PyLong_Check(v)) {
      return PyLong_FromLong(SDDS_SetParameters(&dataset_f[fileIndex],SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,index,PyLong_AsLongLong(v),-1));
    } else {
      return PyLong_FromLong(SDDS_SetParameters(&dataset_f[fileIndex],SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,index,PyInt_AsLongLong(v),-1));
    }
  case SDDS_ULONG64:
    if (PyLong_Check(v)) {
      return PyLong_FromLong(SDDS_SetParameters(&dataset_f[fileIndex],SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,index,PyLong_AsUnsignedLongLong(v),-1));
    } else {
      return PyLong_FromLong(SDDS_SetParameters(&dataset_f[fileIndex],SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,index,PyInt_AsUnsignedLongLong(v),-1));
    }
  case SDDS_FLOAT:
  case SDDS_DOUBLE:
    return PyLong_FromLong(SDDS_SetParameters(&dataset_f[fileIndex],SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,index,PyFloat_AsDouble(v),-1));
  case SDDS_CHARACTER:
    return PyLong_FromLong(SDDS_SetParameters(&dataset_f[fileIndex],SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,index,(char)(PyString_AsString(v)[0]),-1));
  case SDDS_STRING:
    return PyLong_FromLong(SDDS_SetParameters(&dataset_f[fileIndex],SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,index,PyString_AsString(v),-1));
  }
  return PyLong_FromLong(0);
}

/**
 * @brief Sets an array value in a dataset.
 *
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *        - fileIndex: Index of the dataset file.
 *        - indexOrName: Index or name of the array.
 *        - v: Array values.
 *        - dim: Array dimensions.
 * 
 * @return PyObject*:
 *         - 0 on error.
 *         - 1 on success.
 */
static PyObject* sddsdata_SetArray( PyObject* self, PyObject* args )
{
  long fileIndex;
  long elements;
  PyObject *indexOrName;
  PyObject *v;
  PyObject *dim;
  long type;
  long i;
  void *data = NULL;
  long index;
  long result=0;
  PyObject *temp;
  ARRAY_DEFINITION *arraydef;
  char **names=NULL;
  int32_t number;
  int32_t dimensions;
  int32_t *dimension=NULL;

  if (!PyArg_ParseTuple(args, "lOOO", &fileIndex, &indexOrName, &v, &dim)) {
    return 0;
  }
  if (PyString_Check(indexOrName)) {
    index = SDDS_GetArrayIndex(&dataset_f[fileIndex], (char*)PyString_AsString(indexOrName));
    if ((arraydef = SDDS_GetArrayDefinition(&dataset_f[fileIndex],  (char*)PyString_AsString(indexOrName))) == NULL)
      return 0;
  } else if (PyNumber_Check(indexOrName)) {
    if (PyInt_Check(indexOrName))
      index = PyInt_AsLong(indexOrName);
    else if (PyLong_Check(indexOrName))
      index = PyLong_AsLong(indexOrName);
    else
      return 0;
    names = SDDS_GetArrayNames(&dataset_f[fileIndex], &number);
    if (!(names))
      return 0;
    if (number <= index)
      return 0;
    if ((arraydef = SDDS_GetArrayDefinition(&dataset_f[fileIndex], names[index])) == NULL)
      return 0;
  } else
    return 0;
  type = arraydef->type;
  dimensions = arraydef->dimensions;
  
  if (dimensions != PyList_Size(dim))
    return 0;
  dimension = malloc(sizeof(int32_t)*dimensions);

  for (i=0;i<dimensions;i++) {
    temp = PyList_GetItem(dim, i);
    if (PyInt_Check(temp))
      dimension[i] = PyInt_AsLong(temp);
    else if (PyLong_Check(temp))
      dimension[i] = PyLong_AsLong(temp);
    else
      return 0;
  }

  elements = (long)PyList_Size(v);
  switch (type) {
  case SDDS_SHORT:
    data = malloc(sizeof(short)*elements);
    for (i=0;i<elements;i++) {
      temp = PyList_GetItem(v, i);
      if (PyLong_Check(temp))
        ((short*)data)[i] = (short)PyLong_AsLong(temp);
      else
        ((short*)data)[i] = (short)PyInt_AsLong(temp);
      }
    break;
  case SDDS_USHORT:
    data = malloc(sizeof(unsigned short)*elements);
    for (i=0;i<elements;i++) {
      temp = PyList_GetItem(v, i);
      if (PyLong_Check(temp))
      	((unsigned short*)data)[i] = (unsigned short)PyLong_AsLong(temp);
      else
        ((unsigned short*)data)[i] = (unsigned short)PyInt_AsLong(temp);
      }
    break;
  case SDDS_LONG:
    data = malloc(sizeof(int32_t)*elements);
    for (i=0;i<elements;i++) {
      temp = PyList_GetItem(v, i);
      if (PyLong_Check(temp))
        ((int32_t*)data)[i] = (int32_t)PyLong_AsLong(temp);
      else
        ((int32_t*)data)[i] = (int32_t)PyInt_AsLong(temp);
    }
    break;
  case SDDS_ULONG:
    data = malloc(sizeof(uint32_t)*elements);
    for (i=0;i<elements;i++) {
      temp = PyList_GetItem(v, i);
      if (PyLong_Check(temp))
        ((uint32_t*)data)[i] = (uint32_t)PyLong_AsUnsignedLong(temp);
      else
        ((uint32_t*)data)[i] = (uint32_t)PyInt_AsUnsignedLong(temp);
    }
    break;
  case SDDS_LONG64:
    data = malloc(sizeof(int64_t)*elements);
    for (i=0;i<elements;i++) {
      temp = PyList_GetItem(v, i);
      if (PyLong_Check(temp))
        ((int64_t*)data)[i] = (int64_t)PyLong_AsLongLong(temp);
      else
        ((int64_t*)data)[i] = (int64_t)PyInt_AsLongLong(temp);
    }
    break;
  case SDDS_ULONG64:
    data = malloc(sizeof(uint64_t)*elements);
    for (i=0;i<elements;i++) {
      temp = PyList_GetItem(v, i);
      if (PyLong_Check(temp))
        ((uint64_t*)data)[i] = (uint64_t)PyLong_AsUnsignedLongLong(temp);
      else
        ((uint64_t*)data)[i] = (uint64_t)PyInt_AsUnsignedLongLong(temp);
    }
    break;
  case SDDS_FLOAT:
    data = malloc(sizeof(float)*elements);
    for (i=0;i<elements;i++)
      ((float*)data)[i] = (float)PyFloat_AsDouble(PyList_GetItem(v, i));
    break;
  case SDDS_DOUBLE:
    data = malloc(sizeof(double)*elements);
    for (i=0;i<elements;i++)
      ((double*)data)[i] = (double)PyFloat_AsDouble(PyList_GetItem(v, i));
    break;
  case SDDS_CHARACTER:
    data = malloc(sizeof(char)*elements);
    for (i=0;i<elements;i++)
      ((char*)data)[i] = (char)(PyString_AsString(PyList_GetItem(v, i))[0]);
    break;
  case SDDS_STRING:
    data = malloc(sizeof(char*)*elements);
    for (i=0;i<elements;i++)
      SDDS_CopyString(((char**)data)+i, PyString_AsString(PyList_GetItem(v, i)));
    break;
  }
  if (PyString_Check(indexOrName)) {
    result = SDDS_SetArray(&dataset_f[fileIndex],(char*)PyString_AsString(indexOrName),SDDS_CONTIGUOUS_DATA,data,dimension);
  } else {
    result = SDDS_SetArray(&dataset_f[fileIndex],names[index],SDDS_CONTIGUOUS_DATA,data,dimension);
    for (i=0;i<number;i++)
      free(names[i]);
    free(names);
  }
  switch (type) {
  case SDDS_SHORT:
    free((short*)data);
    break;
  case SDDS_USHORT:
    free((unsigned short*)data);
    break;
  case SDDS_LONG:
    free((int32_t*)data);
    break;
  case SDDS_ULONG:
    free((uint32_t*)data);
    break;
  case SDDS_LONG64:
    free((int64_t*)data);
    break;
  case SDDS_ULONG64:
    free((uint64_t*)data);
    break;
  case SDDS_FLOAT:
    free((float*)data);
    break;
  case SDDS_DOUBLE:
    free((double*)data);
    break;
  case SDDS_CHARACTER:
    free((char*)data);
    break;
  case SDDS_STRING:
    for (i=0;i<elements;i++)
      free(((char**)data)[i]);
    free((char**)data);
    break;
  }
  if (dimension) free(dimension);
  return PyLong_FromLong(result);
}

/**
 * @brief Sets a column value in a dataset.
 *
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *        - fileIndex: Index of the dataset file.
 *        - indexOrName: Index or name of the column.
 *        - v: List of values to set for the column.
 * 
 * @return PyObject*:
 *         - 0 on error.
 *         - 1 on success.
 */
static PyObject* sddsdata_SetColumn( PyObject* self, PyObject* args )
{
  long fileIndex;
  long rows;
  PyObject *indexOrName;
  PyObject *v;
  long type;
  long i;
  void *data = NULL;
  long index;
  long result=0;
  PyObject *temp;
  if (!PyArg_ParseTuple(args, "lOO", &fileIndex, &indexOrName, &v)) {
    return 0;
  }
  if (PyString_Check(indexOrName)) {
    index = SDDS_GetColumnIndex(&dataset_f[fileIndex], (char*)PyString_AsString(indexOrName));
  } else if (PyNumber_Check(indexOrName)) {
    if (PyInt_Check(indexOrName))
      index = PyInt_AsLong(indexOrName);
    else if (PyLong_Check(indexOrName))
      index = PyLong_AsLong(indexOrName);
    else
      return 0;
  } else
    return 0;
  if ((type = SDDS_GetColumnType(&dataset_f[fileIndex], index)) == 0)
    return 0;
  rows = (long)PyList_Size(v);
  switch (type) {
  case SDDS_SHORT:
    data = malloc(sizeof(short)*rows);
    for (i=0;i<rows;i++) {
      temp = PyList_GetItem(v, i);
      if (PyLong_Check(temp))
        ((short*)data)[i] = (short)PyLong_AsLong(temp);
      else
        ((short*)data)[i] = (short)PyInt_AsLong(temp);
      }
    break;
  case SDDS_USHORT:
    data = malloc(sizeof(unsigned short)*rows);
    for (i=0;i<rows;i++) {
      temp = PyList_GetItem(v, i);
      if (PyLong_Check(temp))
        ((unsigned short*)data)[i] = (unsigned short)PyLong_AsLong(temp);
      else
        ((unsigned short*)data)[i] = (unsigned short)PyInt_AsLong(temp);
      }
    break;
  case SDDS_LONG:
    data = malloc(sizeof(int32_t)*rows);
    for (i=0;i<rows;i++) {
      temp = PyList_GetItem(v, i);
      if (PyLong_Check(temp))
        ((int32_t*)data)[i] = (int32_t)PyLong_AsLong(temp);
      else
        ((int32_t*)data)[i] = (int32_t)PyInt_AsLong(temp);
    }
    break;
  case SDDS_ULONG:
    data = malloc(sizeof(uint32_t)*rows);
    for (i=0;i<rows;i++) {
      temp = PyList_GetItem(v, i);
      if (PyLong_Check(temp))
        ((uint32_t*)data)[i] = (uint32_t)PyLong_AsUnsignedLong(temp);
      else
        ((uint32_t*)data)[i] = (uint32_t)PyInt_AsUnsignedLong(temp);
    }
    break;
  case SDDS_LONG64:
    data = malloc(sizeof(int64_t)*rows);
    for (i=0;i<rows;i++) {
      temp = PyList_GetItem(v, i);
      if (PyLong_Check(temp))
        ((int64_t*)data)[i] = (int64_t)PyLong_AsLongLong(temp);
      else
        ((int64_t*)data)[i] = (int64_t)PyInt_AsLongLong(temp);
    }
    break;
  case SDDS_ULONG64:
    data = malloc(sizeof(uint64_t)*rows);
    for (i=0;i<rows;i++) {
      temp = PyList_GetItem(v, i);
      if (PyLong_Check(temp))
        ((uint64_t*)data)[i] = (uint64_t)PyLong_AsUnsignedLongLong(temp);
      else
        ((uint64_t*)data)[i] = (uint64_t)PyInt_AsUnsignedLongLong(temp);
    }
    break;
  case SDDS_FLOAT:
    data = malloc(sizeof(float)*rows);
    for (i=0;i<rows;i++)
      ((float*)data)[i] = (float)PyFloat_AsDouble(PyList_GetItem(v, i));
    break;
  case SDDS_DOUBLE:
    data = malloc(sizeof(double)*rows);
    for (i=0;i<rows;i++)
      ((double*)data)[i] = (double)PyFloat_AsDouble(PyList_GetItem(v, i));
    break;
  case SDDS_CHARACTER:
    data = malloc(sizeof(char)*rows);
    for (i=0;i<rows;i++)
      ((char*)data)[i] = (char)(PyString_AsString(PyList_GetItem(v, i))[0]);
    break;
  case SDDS_STRING:
    data = malloc(sizeof(char*)*rows);
    for (i=0;i<rows;i++)
      SDDS_CopyString(((char**)data)+i, PyString_AsString(PyList_GetItem(v, i)));
    break;
  }
  result = SDDS_SetColumn(&dataset_f[fileIndex],SDDS_SET_BY_INDEX,data,rows,index,NULL);
  switch (type) {
  case SDDS_SHORT:
    free((short*)data);
    break;
  case SDDS_USHORT:
    free((unsigned short*)data);
    break;
  case SDDS_LONG:
    free((int32_t*)data);
    break;
  case SDDS_ULONG:
    free((uint32_t*)data);
    break;
  case SDDS_LONG64:
    free((int64_t*)data);
    break;
  case SDDS_ULONG64:
    free((uint64_t*)data);
    break;
  case SDDS_FLOAT:
    free((float*)data);
    break;
  case SDDS_DOUBLE:
    free((double*)data);
    break;
  case SDDS_CHARACTER:
    free((char*)data);
    break;
  case SDDS_STRING:
    for (i=0;i<rows;i++)
      free(((char**)data)[i]);
    free((char**)data);
    break;
  }
  return PyLong_FromLong(result);
}

/**
 * @brief Sets values for a specific row in a dataset.
 *
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *        - fileIndex: Index of the dataset file.
 *        - row: Row number to set values for.
 *        - v: List of column-value pairs (alternating column name and value).
 * 
 * @return PyObject*:
 *         - 0 on error.
 *         - 1 on success.
 */
static PyObject* sddsdata_SetRowValues( PyObject* self, PyObject* args )
{
  long fileIndex;
  long row, elements;
  PyObject *v;
  long type;
  long i;
  long index;
  PyObject *temp;
  if (!PyArg_ParseTuple(args, "llO", &fileIndex, &row, &v)) {
    return 0;
  }
  elements = (long)PyList_Size(v);
  for (i=0; i<elements; i=i+2) {
    index = SDDS_GetColumnIndex(&dataset_f[fileIndex], (char*)PyString_AsString(PyList_GetItem(v, i)));
    temp = PyList_GetItem(v, i+1);
    if ((type = SDDS_GetColumnType(&dataset_f[fileIndex], index)) == 0)
      return 0;
    switch (type) {
    case SDDS_SHORT:
    case SDDS_USHORT:
    case SDDS_LONG:
      if (PyLong_Check(temp)) {
        if (SDDS_SetRowValues(&dataset_f[fileIndex],SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,row,index,PyLong_AsLong(temp),-1) == 0)
          return 0;
      } else {
        if (SDDS_SetRowValues(&dataset_f[fileIndex],SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,row,index,PyInt_AsLong(temp),-1) == 0)
          return 0;
      }
      break;
    case SDDS_ULONG:
      if (PyLong_Check(temp)) {
        if (SDDS_SetRowValues(&dataset_f[fileIndex],SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,row,index,PyLong_AsUnsignedLong(temp),-1) == 0)
          return 0;
      } else {
        if (SDDS_SetRowValues(&dataset_f[fileIndex],SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,row,index,PyInt_AsUnsignedLong(temp),-1) == 0)
          return 0;
      }
      break;
    case SDDS_LONG64:
      if (PyLong_Check(temp)) {
        if (SDDS_SetRowValues(&dataset_f[fileIndex],SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,row,index,PyLong_AsLongLong(temp),-1) == 0)
          return 0;
      } else {
        if (SDDS_SetRowValues(&dataset_f[fileIndex],SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,row,index,PyInt_AsLongLong(temp),-1) == 0)
          return 0;
      }
      break;
    case SDDS_ULONG64:
      if (PyLong_Check(temp)) {
        if (SDDS_SetRowValues(&dataset_f[fileIndex],SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,row,index,PyLong_AsUnsignedLongLong(temp),-1) == 0)
          return 0;
      } else {
        if (SDDS_SetRowValues(&dataset_f[fileIndex],SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,row,index,PyInt_AsUnsignedLongLong(temp),-1) == 0)
          return 0;
      }
      break;
    case SDDS_FLOAT:
    case SDDS_DOUBLE:
      if (SDDS_SetRowValues(&dataset_f[fileIndex],SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,row,index,PyFloat_AsDouble(temp),-1) == 0)
          return 0;
      break;
    case SDDS_CHARACTER:
      if (SDDS_SetRowValues(&dataset_f[fileIndex],SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,row,index,(char)(PyString_AsString(temp)[0]),-1) == 0)
          return 0;
      break;
    case SDDS_STRING:
      if (SDDS_SetRowValues(&dataset_f[fileIndex],SDDS_SET_BY_INDEX|SDDS_PASS_BY_VALUE,row,index,PyString_AsString(temp),-1) == 0)
          return 0;
      break;
    }
  }
  return PyLong_FromLong(1);
}

/**
 * @brief Retrieves a column's data from a dataset.
 *
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *        - fileIndex: Index of the dataset file.
 *        - indexOrName: Index or name of the column to retrieve.
 * 
 * @return PyObject*:
 *         - NULL on error.
 *         - List of values from the column on success.
 */
static PyObject* sddsdata_GetColumn( PyObject* self, PyObject* args )
{
  long fileIndex;
  PyObject *indexOrName;
  long originalType;
  int64_t rows, i;
  void *columnValue;
  char *name;
  char **data;
  int32_t number;
  char buffer[40];
  long index;
  PyObject *v;
  if (!PyArg_ParseTuple(args, "lO", &fileIndex, &indexOrName)) {
    return NULL;
  }
  if (PyString_Check(indexOrName)) {
    index = SDDS_GetColumnIndex(&dataset_f[fileIndex], (char*)PyString_AsString(indexOrName));
  } else if (PyNumber_Check(indexOrName)) {
    if (PyInt_Check(indexOrName))
      index = PyInt_AsLong(indexOrName);
    else if (PyLong_Check(indexOrName))
      index = PyLong_AsLong(indexOrName);
    else
      return NULL;
  } else
    return NULL;
  
  data = SDDS_GetColumnNames(&dataset_f[fileIndex], &number);
  if (!(data))
    return NULL;
  if ((index >= 0) && (index < number)) {
    name = data[index];
  } else {
    return NULL;
  }

  rows = SDDS_RowCount(&(dataset_f[fileIndex]));
  if (rows < 0) {
    Py_INCREF(Py_None);
    return Py_None;
  }

  originalType = SDDS_GetColumnType(&(dataset_f[fileIndex]), index);
    
  columnValue = SDDS_GetColumn(&(dataset_f[fileIndex]), name);

  for (i=0;i<number;i++)
    free(data[i]);
  free(data);

  if (!(columnValue))
    return NULL;

  if (!(v = PyList_New(rows)))
    return NULL;
  
  switch (originalType) {
  case SDDS_SHORT:
    for (i=0;i < rows;i++) {
      PyList_SetItem(v, i, PyLong_FromLong((long)(((short*)columnValue)[i])));
    }
    free((short*)columnValue);
    break;
  case SDDS_USHORT:
    for (i=0;i < rows;i++) {
      PyList_SetItem(v, i, PyLong_FromLong((long)(((unsigned short*)columnValue)[i])));
    }
    free((unsigned short*)columnValue);
    break;
  case SDDS_LONG:
    for (i=0;i < rows;i++) {
      PyList_SetItem(v, i, PyLong_FromLong((long)(((int32_t*)columnValue)[i])));
    }
    free((int32_t*)columnValue);
    break;
  case SDDS_ULONG:
    for (i=0;i < rows;i++) {
      PyList_SetItem(v, i, PyLong_FromUnsignedLong((unsigned long)(((uint32_t*)columnValue)[i])));
    }
    free((uint32_t*)columnValue);
    break;
  case SDDS_LONG64:
    for (i=0;i < rows;i++) {
      PyList_SetItem(v, i, PyLong_FromLongLong((long long)(((int64_t*)columnValue)[i])));
    }
    free((int64_t*)columnValue);
    break;
  case SDDS_ULONG64:
    for (i=0;i < rows;i++) {
      PyList_SetItem(v, i, PyLong_FromUnsignedLongLong((unsigned long long)(((uint64_t*)columnValue)[i])));
    }
    free((uint64_t*)columnValue);
    break;
  case SDDS_FLOAT:
    for (i=0;i < rows;i++) {
      sprintf(buffer, "%.6E", ((float*)columnValue)[i]);
      PyList_SetItem(v, i, PyFloat_FromDouble(atof(buffer)));
    }
    free((float*)columnValue);
    break;
  case SDDS_DOUBLE:
    for (i=0;i < rows;i++) {
      PyList_SetItem(v, i, PyFloat_FromDouble(((double*)columnValue)[i]));
    }
    free((double*)columnValue);
    break;
  case SDDS_CHARACTER:
    for (i=0;i < rows;i++) {
      sprintf(buffer, "%c", ((char*)columnValue)[i]);
      PyList_SetItem(v, i, PyString_FromString(buffer));
    }
    free((char*)columnValue);
    break;
  case SDDS_STRING:
    for (i=0;i < rows;i++) {
      PyList_SetItem(v, i, PyString_FromString(((char**)columnValue)[i]));
    }
    for (i=0;i < rows;i++)
      free(((char**)columnValue)[i]);
    free((char**)columnValue);
    break;
  default:
    return NULL;
  }
  return v;
}

/**
 * @brief Retrieves an array's data from a dataset.
 *
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *        - fileIndex: Index of the dataset file.
 *        - indexOrName: Index or name of the array to retrieve.
 * 
 * @return PyObject*:
 *         - NULL on error.
 *         - List of values from the array on success.
 */
static PyObject* sddsdata_GetArray( PyObject* self, PyObject* args )
{
  long fileIndex;
  PyObject *indexOrName;
  long originalType, i, elements;
  SDDS_ARRAY *arrayValue=NULL;
  char *name;
  char **data;
  int32_t number;
  char buffer[40];
  long index;
  PyObject *v;
  if (!PyArg_ParseTuple(args, "lO", &fileIndex, &indexOrName)) {
    return NULL;
  }
  if (PyString_Check(indexOrName)) {
    index = SDDS_GetArrayIndex(&dataset_f[fileIndex], (char*)PyString_AsString(indexOrName));
  } else if (PyNumber_Check(indexOrName)) {
    if (PyInt_Check(indexOrName))
      index = PyInt_AsLong(indexOrName);
    else if (PyLong_Check(indexOrName))
      index = PyLong_AsLong(indexOrName);
    else
      return NULL;
  } else
    return NULL;
  
  data = SDDS_GetArrayNames(&dataset_f[fileIndex], &number);
  if (!(data))
    return NULL;
  if ((index >= 0) && (index < number)) {
    name = data[index];
  } else {
    return NULL;
  }

  arrayValue = SDDS_GetArray(&(dataset_f[fileIndex]), name, NULL);

  for (i=0;i<number;i++)
    free(data[i]);
  free(data);

  originalType = arrayValue->definition->type;

  if (!(arrayValue))
    return NULL;

  elements = arrayValue->elements;

  /*
  if (!(v = PyList_New(arrayValue->definition->dimensions)))
    return NULL;
  for (i=0;i<arrayValue>dimensions;i++) {
    if (!(v[i] = PyList_New(arrayValue->dimension[i])))
      return NULL;
  }
  */
  if (!(v = PyList_New(arrayValue->elements)))
    return NULL;

  switch (originalType) {
  case SDDS_SHORT:
    for (i=0;i < elements;i++) {
      PyList_SetItem(v, i, PyLong_FromLong((long)(((short*)arrayValue->data)[i])));
    }
    break;
  case SDDS_USHORT:
    for (i=0;i < elements;i++) {
      PyList_SetItem(v, i, PyLong_FromLong((long)(((unsigned short*)arrayValue->data)[i])));
    }
    break;
  case SDDS_LONG:
    for (i=0;i < elements;i++) {
      PyList_SetItem(v, i, PyLong_FromLong((long)(((int32_t*)arrayValue->data)[i])));
    }
    break;
  case SDDS_ULONG:
    for (i=0;i < elements;i++) {
      PyList_SetItem(v, i, PyLong_FromUnsignedLong((unsigned long)(((uint32_t*)arrayValue->data)[i])));
    }
    break;
  case SDDS_LONG64:
    for (i=0;i < elements;i++) {
      PyList_SetItem(v, i, PyLong_FromLongLong((long long)(((int64_t*)arrayValue->data)[i])));
    }
    break;
  case SDDS_ULONG64:
    for (i=0;i < elements;i++) {
      PyList_SetItem(v, i, PyLong_FromUnsignedLongLong((unsigned long long)(((uint64_t*)arrayValue->data)[i])));
    }
    break;
  case SDDS_FLOAT:
    for (i=0;i < elements;i++) {
      sprintf(buffer, "%.6E", ((float*)arrayValue->data)[i]);
      PyList_SetItem(v, i, PyFloat_FromDouble(atof(buffer)));
    }
    break;
  case SDDS_DOUBLE:
    for (i=0;i < elements;i++) {
      PyList_SetItem(v, i, PyFloat_FromDouble(((double*)arrayValue->data)[i]));
    }
    break;
  case SDDS_CHARACTER:
    for (i=0;i < elements;i++) {
      sprintf(buffer, "%c", ((char*)arrayValue->data)[i]);
      PyList_SetItem(v, i, PyString_FromString(buffer));
    }
    break;
  case SDDS_STRING:
    for (i=0;i < elements;i++) {
      PyList_SetItem(v, i, PyString_FromString(((char**)arrayValue->data)[i]));
    }
    break;
  default:
    return NULL;
  }
  SDDS_FreeArray(arrayValue);
  return v;
}

/**
 * @brief Retrieves the dimensions of an array in a dataset.
 *
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *        - fileIndex: Index of the dataset file.
 *        - indexOrName: Index or name of the array.
 * 
 * @return PyObject*:
 *         - NULL on error.
 *         - List of dimensions of the array on success.
 */
static PyObject* sddsdata_GetArrayDimensions( PyObject* self, PyObject* args )
{
  long fileIndex;
  PyObject *indexOrName;
  long i;
  SDDS_ARRAY *arrayValue=NULL;
  char *name;
  char **data;
  int32_t number;
  long index;
  PyObject *v;
  if (!PyArg_ParseTuple(args, "lO", &fileIndex, &indexOrName)) {
    return NULL;
  }
  if (PyString_Check(indexOrName)) {
    index = SDDS_GetArrayIndex(&dataset_f[fileIndex], (char*)PyString_AsString(indexOrName));
  } else if (PyNumber_Check(indexOrName)) {
    if (PyInt_Check(indexOrName))
      index = PyInt_AsLong(indexOrName);
    else if (PyLong_Check(indexOrName))
      index = PyLong_AsLong(indexOrName);
    else
      return NULL;
  } else
    return NULL;
  
  data = SDDS_GetArrayNames(&dataset_f[fileIndex], &number);
  if (!(data))
    return NULL;
  if ((index >= 0) && (index < number)) {
    name = data[index];
  } else {
    return NULL;
  }

  arrayValue = SDDS_GetArray(&(dataset_f[fileIndex]), name, NULL);

  for (i=0;i<number;i++)
    free(data[i]);
  free(data);

  if (!(arrayValue))
    return NULL;
  
  if (!(v = PyList_New(arrayValue->definition->dimensions)))
    return NULL;
  for (i=0;i<arrayValue->definition->dimensions;i++) {
    PyList_SetItem(v, i, PyLong_FromLong(arrayValue->dimension[i]));
  }
  SDDS_FreeArray(arrayValue);
  return v;
}

/**
 * @brief Retrieves the value of a parameter from a dataset.
 *
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *        - fileIndex: Index of the dataset file.
 *        - indexOrName: Index or name of the parameter.
 * 
 * @return PyObject*:
 *         - NULL on error.
 *         - Parameter value on success.
 */
static PyObject* sddsdata_GetParameter( PyObject* self, PyObject* args ) 
{
  long fileIndex;
  PyObject *indexOrName;
  long originalType;
  void *parameterValue;
  char *name;
  char **data;
  int32_t number;
  char buffer[40];
  long index;
  long i;
  PyObject *v=NULL;
  if (!PyArg_ParseTuple(args, "lO", &fileIndex, &indexOrName)) {
    return NULL;
  }
  if (PyString_Check(indexOrName)) {
    index = SDDS_GetParameterIndex(&dataset_f[fileIndex], (char*)PyString_AsString(indexOrName));
  } else if (PyNumber_Check(indexOrName)) {
    if (PyInt_Check(indexOrName))
      index = PyInt_AsLong(indexOrName);
    else if (PyLong_Check(indexOrName))
      index = PyLong_AsLong(indexOrName);
    else
      return NULL;
  } else
    return NULL;

  data = SDDS_GetParameterNames(&dataset_f[fileIndex], &number);
  if (!(data))
    return NULL;
  if ((index >= 0) && (index < number)) {
    name = data[index];
  } else {
    return NULL;
  }
  if (!name) {
    return NULL;
  }

  parameterValue = SDDS_GetParameter(&(dataset_f[fileIndex]), name, NULL);

  for (i=0;i<number;i++)
    free(data[i]);
  free(data);

  if (!(parameterValue)) {
    return NULL;
  }
  originalType = SDDS_GetParameterType(&dataset_f[fileIndex], index);

  switch (originalType) {
  case SDDS_SHORT:
    v = PyLong_FromLong((long)(((short*)parameterValue)[0]));
    free((short*)parameterValue);
    break;
  case SDDS_USHORT:
    v = PyLong_FromLong((long)(((unsigned short*)parameterValue)[0]));
    free((unsigned short*)parameterValue);
    break;
  case SDDS_LONG:
    v = PyLong_FromLong((long)(((int32_t*)parameterValue)[0]));
    free((int32_t*)parameterValue);
    break;
  case SDDS_ULONG:
    v = PyLong_FromUnsignedLong((unsigned long)(((uint32_t*)parameterValue)[0]));
    free((uint32_t*)parameterValue);
    break;
  case SDDS_LONG64:
    v = PyLong_FromLongLong((long long)(((int64_t*)parameterValue)[0]));
    free((int64_t*)parameterValue);
    break;
  case SDDS_ULONG64:
    v = PyLong_FromUnsignedLongLong((unsigned long long)(((uint64_t*)parameterValue)[0]));
    free((uint64_t*)parameterValue);
    break;
  case SDDS_FLOAT:
    sprintf(buffer, "%.6E", ((float*)parameterValue)[0]);
    v = PyFloat_FromDouble(atof(buffer));
    free((float*)parameterValue);
    break;
  case SDDS_DOUBLE:
    v = PyFloat_FromDouble(((double*)parameterValue)[0]);
    free((double*)parameterValue);
    break;
  case SDDS_CHARACTER:
    sprintf(buffer, "%c", ((char*)parameterValue)[0]);
    v = PyString_FromString(buffer);
    free((char*)parameterValue);
    break;
  case SDDS_STRING:
    v = PyString_FromString(((char**)parameterValue)[0]);
    free(((char**)parameterValue)[0]);
    free((char**)parameterValue);
    break;
  }
  return v;
}

/**
 * @brief Retrieves the data mode of a dataset.
 *
 * @param self Python object (unused).
 * @param args Python tuple containing:
 *        - fileIndex: Index of the dataset file.
 * 
 * @return PyObject*:
 *         - Data mode of the dataset.
 */
static PyObject* sddsdata_GetMode( PyObject* self, PyObject* args ) 
{
  long fileIndex;
  if (!PyArg_ParseTuple(args, "l", &fileIndex)) {
    return 0;
  }
  return PyLong_FromLong(dataset_f[fileIndex].layout.data_mode.mode);
}

struct module_state {
    PyObject *error;
};

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct module_state*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct module_state _state;
#endif

static PyMethodDef sddsdata_methods[] = {
  { "InitializeInput", sddsdata_InitializeInput, METH_VARARGS },
  { "InitializeAppend", sddsdata_InitializeAppend, METH_VARARGS },
  { "InitializeAppendToPage", sddsdata_InitializeAppendToPage, METH_VARARGS },
  { "InitializeOutput", sddsdata_InitializeOutput, METH_VARARGS },
  { "SetColumnMajorOrder", sddsdata_SetColumnMajorOrder, METH_VARARGS },
  { "SetRowMajorOrder", sddsdata_SetRowMajorOrder, METH_VARARGS },
  { "SetFixedRowCountMode", sddsdata_SetFixedRowCountMode, METH_VARARGS },
  { "EnableFSync", sddsdata_EnableFSync, METH_VARARGS },
  { "DisableFSync", sddsdata_DisableFSync, METH_VARARGS },
  { "Terminate", sddsdata_Terminate, METH_VARARGS },
  { "SetTerminateMode", sddsdata_SetTerminateMode, METH_VARARGS },
  { "DefineParameter", sddsdata_DefineParameter, METH_VARARGS },
  { "DefineArray", sddsdata_DefineArray, METH_VARARGS },
  { "DefineColumn", sddsdata_DefineColumn, METH_VARARGS },
  { "IsValidName", sddsdata_IsValidName, METH_VARARGS },
  { "SetNameValidityFlags", sddsdata_SetNameValidityFlags, METH_VARARGS },
  { "DefineSimpleColumn", sddsdata_DefineSimpleColumn, METH_VARARGS },
  { "DefineSimpleArray", sddsdata_DefineSimpleArray, METH_VARARGS },
  { "DefineSimpleParameter", sddsdata_DefineSimpleParameter, METH_VARARGS },
  { "WriteLayout", sddsdata_WriteLayout, METH_VARARGS },
  { "EraseData", sddsdata_EraseData, METH_VARARGS },
  { "ProcessColumnString", sddsdata_ProcessColumnString, METH_VARARGS },
  { "ProcessArrayString", sddsdata_ProcessArrayString, METH_VARARGS },
  { "ProcessParameterString", sddsdata_ProcessParameterString, METH_VARARGS },
  { "InitializeCopy", sddsdata_InitializeCopy, METH_VARARGS },
  { "CopyLayout", sddsdata_CopyLayout, METH_VARARGS },
  { "AppendLayout", sddsdata_AppendLayout, METH_VARARGS },
  { "CopyPage", sddsdata_CopyPage, METH_VARARGS },
  { "CopyParameters", sddsdata_CopyParameters, METH_VARARGS },
  { "CopyArrays", sddsdata_CopyArrays, METH_VARARGS },
  { "CopyColumns", sddsdata_CopyColumns, METH_VARARGS },
  { "CopyRow", sddsdata_CopyRow, METH_VARARGS },
  { "CopyRowDirect", sddsdata_CopyRowDirect, METH_VARARGS },
  { "CopyAdditionalRows", sddsdata_CopyAdditionalRows, METH_VARARGS },
  { "DeferSavingLayout", sddsdata_DeferSavingLayout, METH_VARARGS },
  { "SaveLayout", sddsdata_SaveLayout, METH_VARARGS },
  { "RestoreLayout", sddsdata_RestoreLayout, METH_VARARGS },
  { "StartPage", sddsdata_StartPage, METH_VARARGS },
  { "ClearPage", sddsdata_ClearPage, METH_VARARGS },
  { "LengthenTable", sddsdata_LengthenTable, METH_VARARGS },
  { "WritePage", sddsdata_WritePage, METH_VARARGS },
  { "UpdatePage", sddsdata_UpdatePage, METH_VARARGS },
  { "InitHeaderlessInput", sddsdata_InitHeaderlessInput, METH_VARARGS },
  { "ReadPage", sddsdata_ReadPage, METH_VARARGS },
  { "ReadPageSparse", sddsdata_ReadPageSparse, METH_VARARGS },
  { "ReadPageLastRows", sddsdata_ReadPageLastRows, METH_VARARGS },
  { "RowCount", sddsdata_RowCount, METH_VARARGS },
  { "SetColumnFlags", sddsdata_SetColumnFlags, METH_VARARGS },
  { "SetRowFlags", sddsdata_SetRowFlags, METH_VARARGS },
  { "GetRowFlag", sddsdata_GetRowFlag, METH_VARARGS },
  { "DeleteColumn", sddsdata_DeleteColumn, METH_VARARGS },
  { "DeleteParameter", sddsdata_DeleteParameter, METH_VARARGS },
  { "DeleteUnsetColumns", sddsdata_DeleteUnsetColumns, METH_VARARGS },
  { "DeleteUnsetRows", sddsdata_DeleteUnsetRows, METH_VARARGS },
  { "ColumnCount", sddsdata_ColumnCount, METH_VARARGS },
  { "ArrayCount", sddsdata_ArrayCount, METH_VARARGS },
  { "ParameterCount", sddsdata_ParameterCount, METH_VARARGS },
  { "GetDescription", sddsdata_GetDescription, METH_VARARGS },
  { "GetDescriptionText", sddsdata_GetDescriptionText, METH_VARARGS },
  { "GetDescriptionContents", sddsdata_GetDescriptionContents, METH_VARARGS },
  { "NumberOfErrors", sddsdata_NumberOfErrors, METH_VARARGS },
  { "ClearErrors", sddsdata_ClearErrors, METH_VARARGS },
  { "SetError", sddsdata_SetError, METH_VARARGS },
  { "Bomb", sddsdata_Bomb, METH_VARARGS },
  { "Warning", sddsdata_Warning, METH_VARARGS },
  { "RegisterProgramName", sddsdata_RegisterProgramName, METH_VARARGS },
  { "PrintErrors", sddsdata_PrintErrors, METH_VARARGS },
  { "TransferColumnDefinition", sddsdata_TransferColumnDefinition, METH_VARARGS },
  { "TransferArrayDefinition", sddsdata_TransferArrayDefinition, METH_VARARGS },
  { "TransferParameterDefinition", sddsdata_TransferParameterDefinition, METH_VARARGS },
  { "DefineColumnLikeParameter", sddsdata_DefineColumnLikeParameter, METH_VARARGS },
  { "DefineParameterLikeColumn", sddsdata_DefineParameterLikeColumn, METH_VARARGS },
  { "TransferAllColumnDefinitions", sddsdata_TransferAllColumnDefinitions, METH_VARARGS },
  { "TransferAllArrayDefinitions", sddsdata_TransferAllArrayDefinitions, METH_VARARGS },
  { "TransferAllParameterDefinitions", sddsdata_TransferAllParameterDefinitions, METH_VARARGS },
  { "GetColumnIndex", sddsdata_GetColumnIndex, METH_VARARGS },
  { "GetArrayIndex", sddsdata_GetArrayIndex, METH_VARARGS },
  { "GetParameterIndex", sddsdata_GetParameterIndex, METH_VARARGS },
  { "GetColumnType", sddsdata_GetColumnType, METH_VARARGS },
  { "GetArrayType", sddsdata_GetArrayType, METH_VARARGS },
  { "GetNamedColumnType", sddsdata_GetNamedColumnType, METH_VARARGS },
  { "GetNamedArrayType", sddsdata_GetNamedArrayType, METH_VARARGS },
  { "GetColumnDefinition", sddsdata_GetColumnDefinition, METH_VARARGS },
  { "GetArrayDefinition", sddsdata_GetArrayDefinition, METH_VARARGS },
  { "GetParameterType", sddsdata_GetParameterType, METH_VARARGS },
  { "GetNamedParameterType", sddsdata_GetNamedParameterType, METH_VARARGS },
  { "GetParameterDefinition", sddsdata_GetParameterDefinition, METH_VARARGS },
  { "GetTypeSize", sddsdata_GetTypeSize, METH_VARARGS },
  { "GetTypeName", sddsdata_GetTypeName, METH_VARARGS },
  { "IdentifyType", sddsdata_IdentifyType, METH_VARARGS },
  { "CheckColumn", sddsdata_CheckColumn, METH_VARARGS },
  { "CheckArray", sddsdata_CheckArray, METH_VARARGS },
  { "CheckParameter", sddsdata_CheckParameter, METH_VARARGS },
  { "HasWhitespace", sddsdata_HasWhitespace, METH_VARARGS },
  { "StringIsBlank", sddsdata_StringIsBlank, METH_VARARGS },
  { "ApplyFactorToParameter", sddsdata_ApplyFactorToParameter, METH_VARARGS },
  { "ApplyFactorToColumn", sddsdata_ApplyFactorToColumn, METH_VARARGS },
  { "DeleteParameterFixedValues", sddsdata_DeleteParameterFixedValues, METH_VARARGS },
  { "SetDataMode", sddsdata_SetDataMode, METH_VARARGS },
  { "CheckDataset", sddsdata_CheckDataset, METH_VARARGS },
  { "SetAutoCheckMode", sddsdata_SetAutoCheckMode, METH_VARARGS },
  { "GetColumnNameFromIndex", sddsdata_GetColumnNameFromIndex, METH_VARARGS },
  { "GetColumnNames", sddsdata_GetColumnNames, METH_VARARGS },
  { "GetArrayNameFromIndex", sddsdata_GetArrayNameFromIndex, METH_VARARGS },
  { "GetArrayNames", sddsdata_GetArrayNames, METH_VARARGS },
  { "GetParameterNameFromIndex", sddsdata_GetParameterNameFromIndex, METH_VARARGS },
  { "GetParameterNames", sddsdata_GetParameterNames, METH_VARARGS },
  { "SetParameter", sddsdata_SetParameter, METH_VARARGS },
  { "SetColumn", sddsdata_SetColumn, METH_VARARGS },
  { "SetArray", sddsdata_SetArray, METH_VARARGS },
  { "SetRowValues", sddsdata_SetRowValues, METH_VARARGS },
  { "GetColumn", sddsdata_GetColumn, METH_VARARGS },
  { "GetArray", sddsdata_GetArray, METH_VARARGS },
  { "GetArrayDimensions", sddsdata_GetArrayDimensions, METH_VARARGS },
  { "GetParameter", sddsdata_GetParameter, METH_VARARGS },
  { "GetMode", sddsdata_GetMode, METH_VARARGS },
  { NULL, NULL }
};

#if PY_MAJOR_VERSION >= 3
 #if defined(_WIN32)
  __declspec(dllexport) PyObject* PyInit_sddsdata(void);
 #endif
#else
 #if defined(_WIN32)
  __declspec(dllexport) void initsddsdata(void);
 #endif
#endif

#if PY_MAJOR_VERSION >= 3
static int sddsdata_traverse(PyObject *m, visitproc visit, void *arg) {
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}
static int sddsdata_clear(PyObject *m) {
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}
static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "sddsdata",
        NULL,
        sizeof(struct module_state),
        sddsdata_methods,
        NULL,
        sddsdata_traverse,
        sddsdata_clear,
        NULL
};
#define INITERROR return NULL
#else
#define INITERROR return
#endif

#if PY_MAJOR_VERSION >= 3
//PyObject * PyInit_sddsdata(void)
PyMODINIT_FUNC PyInit_sddsdata(void)
#else
void initsddsdata(void)
#endif
{
  struct module_state *st;
#if PY_MAJOR_VERSION >= 3
    PyObject *module = PyModule_Create(&moduledef);
#else
    PyObject *module = Py_InitModule( "sddsdata", sddsdata_methods );
#endif
    if (module == NULL)
        INITERROR;

    st = GETSTATE(module);

    st->error = PyErr_NewException("sddsedir.Error", NULL, NULL);
    if (st->error == NULL) {
        Py_DECREF(module);
        INITERROR;
    }

#if PY_MAJOR_VERSION >= 3
    return module;
#endif

}
