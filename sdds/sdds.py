"""
@file sdds.py
@brief Python script providing the SDDS Python module.
@details
This module provides the `SDDS` class and associated methods to load, manipulate, and save SDDS files.
It supports both ASCII and binary SDDS formats and provides functionality to read, write, and manipulate SDDS data.

Dependencies:
    sddsdata module
"""

try:
    from . import sddsdata
except:
    import sddsdata
import sys
import time
from dataclasses import dataclass
from typing import Optional, Any


class SDDS:
    """
    A class to represent and manipulate SDDS datasets.

    This class provides methods to load, manipulate, and save SDDS data.
    It supports ASCII and binary formats and facilitates operations on parameters, arrays, and columns.

    Attributes:
        index (int): The index of the SDDS dataset (integer from 0 to 1000).
        description (list): List containing the description text and contents of the SDDS file.
        parameterName (list): List of parameter names.
        arrayName (list): List of array names.
        columnName (list): List of column names.
        parameterDefinition (list): List of parameter definitions.
        arrayDefinition (list): List of array definitions.
        arrayDimensions (list): List of array dimensions.
        columnDefinition (list): List of column definitions.
        parameterData (list): List of parameter data values.
        arrayData (list): List of array data values.
        columnData (list): List of column data values.
        mode (int): The data storage mode (`SDDS_ASCII` or `SDDS_BINARY`).
    """

    # Class constants for error printing modes
    SDDS_VERBOSE_PrintErrors = 1     # Verbose error printing mode
    SDDS_EXIT_PrintErrors = 2        # Exit on error printing mode

    # Class constants for SDDS check statuses
    SDDS_CHECK_OKAY = 0              # SDDS check status: okay
    SDDS_CHECK_NONEXISTENT = 1       # SDDS check status: non-existent
    SDDS_CHECK_WRONGTYPE = 2         # SDDS check status: wrong type
    SDDS_CHECK_WRONGUNITS = 3        # SDDS check status: wrong units

    # Class constants for data types
    SDDS_LONGDOUBLE = 1
    SDDS_DOUBLE = 2
    SDDS_REAL64 = 2
    SDDS_FLOAT = 3
    SDDS_REAL32 = 3
    SDDS_LONG64 = 4
    SDDS_INT64 = 4
    SDDS_ULONG64 = 5
    SDDS_UINT64 = 5
    SDDS_LONG = 6
    SDDS_INT32 = 6
    SDDS_ULONG = 7
    SDDS_UINT32 = 7
    SDDS_SHORT = 8
    SDDS_INT16 = 8
    SDDS_USHORT = 9
    SDDS_UINT16 = 9
    SDDS_STRING = 10
    SDDS_CHARACTER = 11
    SDDS_NUM_TYPES = 11

    # Class constants for modes
    SDDS_BINARY = 1
    SDDS_ASCII = 2
    SDDS_FLUSH_TABLE = 1

    _used_indices = set()  # Class-level attribute to track used indices
    _max_index = 1000  # Maximum allowable index

    def __init__(self, index=None):
        """
        Initializes a new SDDS object.

        Args:
            index (int, optional): Integer index of the SDDS object. If not provided, an available
                                   index will be automatically assigned. Must be between 0 and 1000.

        Raises:
            ValueError: If no index is provided and all indices are in use.
        """
        if index is None:
            # Automatically assign an unused index
            for i in range(self._max_index + 1):
                if i not in self._used_indices:
                    self.index = i
                    self._used_indices.add(i)
                    break
            else:
                raise ValueError("All SDDS indices are in use. Cannot create a new SDDS object.")
        elif 0 <= index <= self._max_index:
            if index in self._used_indices:
                raise ValueError(f"Index {index} is already in use.")
            self.index = index
            self._used_indices.add(index)
        else:
            raise ValueError(f"Index {index} must be between 0 and {self._max_index}.")

        # Initialize data storage variables
        self.description = ["", ""]
        self.parameterName = []
        self.arrayName = []
        self.columnName = []
        self.parameterDefinition = []
        self.arrayDefinition = []
        self.arrayDimensions = []
        self.columnDefinition = []
        self.parameterData = []
        self.arrayData = []
        self.columnData = []
        self.mode = self.SDDS_ASCII
        self.loaded_pages = 0

    def __del__(self):
        """
        Destructor to release the index when the object is deleted.
        """
        if hasattr(self, 'index') and self.index in self._used_indices:
            self._used_indices.remove(self.index)
            
    def load(self, input):
        """
        Loads an SDDS file into the SDDS object.

        Args:
            input (str): The input SDDS filename to load.

        Raises:
            Exception: If unable to read the SDDS data.

        This method reads the SDDS file specified by `input`, and populates the object's data structures
        with the parameters, arrays, columns, and their respective data.
        """
        try:
            # Open SDDS file
            if sddsdata.InitializeInput(self.index, input) != 1:
                time.sleep(1)
                if sddsdata.InitializeInput(self.index, input) != 1:
                    raise ValueError("Failed to initialize SDDS input.")

            # Get data storage mode (SDDS_ASCII or SDDS_BINARY)
            self.mode = sddsdata.GetMode(self.index)

            # Get description text and contents
            self.description = sddsdata.GetDescription(self.index)

            # Get parameter names
            self.parameterName = sddsdata.GetParameterNames(self.index)
            numberOfParameters = len(self.parameterName)

            # Get array names
            self.arrayName = sddsdata.GetArrayNames(self.index)
            numberOfArrays = len(self.arrayName)

            # Get column names
            self.columnName = sddsdata.GetColumnNames(self.index)
            numberOfColumns = len(self.columnName)

            # Get parameter definitions
            self.parameterDefinition = [sddsdata.GetParameterDefinition(self.index, name) for name in self.parameterName]

            # Get array definitions
            self.arrayDefinition = [sddsdata.GetArrayDefinition(self.index, name) for name in self.arrayName]

            # Get column definitions
            self.columnDefinition = [sddsdata.GetColumnDefinition(self.index, name) for name in self.columnName]

            # Initialize parameter, array, and column data
            self.parameterData = [[] for _ in range(numberOfParameters)]
            self.arrayData = [[] for _ in range(numberOfArrays)]
            self.columnData = [[] for _ in range(numberOfColumns)]
            self.arrayDimensions = [[] for _ in range(numberOfArrays)]

            # Read in SDDS data
            page = sddsdata.ReadPage(self.index)
            if page != 1:
                raise Exception("Unable to read SDDS data for the first page")
            while page > 0:
                self.loaded_pages += 1
                for i in range(numberOfParameters):
                    self.parameterData[i].append(sddsdata.GetParameter(self.index, i))
                for i in range(numberOfArrays):
                    self.arrayData[i].append(sddsdata.GetArray(self.index, i))
                    self.arrayDimensions[i].append(sddsdata.GetArrayDimensions(self.index, i))
                rows = sddsdata.RowCount(self.index)
                if rows > 0:
                    for i in range(numberOfColumns):
                        self.columnData[i].append(sddsdata.GetColumn(self.index, i))
                else:
                    for i in range(numberOfColumns):
                        self.columnData[i].append([])
                page = sddsdata.ReadPage(self.index)

            # Close SDDS file
            if sddsdata.Terminate(self.index) != 1:
                raise ValueError("Failed to terminate SDDS input.")

        except:
            sddsdata.PrintErrors(self.SDDS_VERBOSE_PrintErrors)
            raise

    def loadSparse(self, input, interval, offset):
        """
        Loads an SDDS file into the SDDS object with sparse data reading.

        Args:
            input (str): The input SDDS filename to load.
            interval (int): Interval between pages to read.
            offset (int): Offset to start reading from.

        Raises:
            Exception: If unable to read the SDDS data.

        This method reads every `interval` pages from the SDDS file, starting from the page at `offset`.
        """
        try:
            # Open SDDS file
            if sddsdata.InitializeInput(self.index, input) != 1:
                time.sleep(1)
                if sddsdata.InitializeInput(self.index, input) != 1:
                    raise ValueError("Failed to initialize SDDS input.")

            # Get data storage mode (SDDS_ASCII or SDDS_BINARY)
            self.mode = sddsdata.GetMode(self.index)

            # Get description text and contents
            self.description = sddsdata.GetDescription(self.index)

            # Get parameter names
            self.parameterName = sddsdata.GetParameterNames(self.index)
            numberOfParameters = len(self.parameterName)

            # Get array names
            self.arrayName = sddsdata.GetArrayNames(self.index)
            numberOfArrays = len(self.arrayName)

            # Get column names
            self.columnName = sddsdata.GetColumnNames(self.index)
            numberOfColumns = len(self.columnName)

            # Get parameter definitions
            self.parameterDefinition = [sddsdata.GetParameterDefinition(self.index, name) for name in self.parameterName]

            # Get array definitions
            self.arrayDefinition = [sddsdata.GetArrayDefinition(self.index, name) for name in self.arrayName]

            # Get column definitions
            self.columnDefinition = [sddsdata.GetColumnDefinition(self.index, name) for name in self.columnName]

            # Initialize parameter, array, and column data
            self.parameterData = [[] for _ in range(numberOfParameters)]
            self.arrayData = [[] for _ in range(numberOfArrays)]
            self.columnData = [[] for _ in range(numberOfColumns)]
            self.arrayDimensions = [[] for _ in range(numberOfArrays)]

            # Read in SDDS data sparsely
            page = sddsdata.ReadPageSparse(self.index, interval, offset)
            if page != 1:
                raise Exception("Unable to read SDDS data for the first page")
            while page > 0:
                for i in range(numberOfParameters):
                    self.parameterData[i].append(sddsdata.GetParameter(self.index, i))
                for i in range(numberOfArrays):
                    self.arrayData[i].append(sddsdata.GetArray(self.index, i))
                    self.arrayDimensions[i].append(sddsdata.GetArrayDimensions(self.index, i))
                rows = sddsdata.RowCount(self.index)
                if rows > 0:
                    for i in range(numberOfColumns):
                        self.columnData[i].append(sddsdata.GetColumn(self.index, i))
                else:
                    for i in range(numberOfColumns):
                        self.columnData[i].append([])
                page = sddsdata.ReadPageSparse(self.index, interval, offset)

            # Close SDDS file
            if sddsdata.Terminate(self.index) != 1:
                raise ValueError("Failed to terminate SDDS input.")

        except:
            sddsdata.PrintErrors(self.SDDS_VERBOSE_PrintErrors)
            raise

    def loadLastRows(self, input, lastrows):
        """
        Loads an SDDS file into the SDDS object, reading only the last few rows.

        Args:
            input (str): The input SDDS filename to load.
            lastrows (int): Number of last rows to read.

        Raises:
            Exception: If unable to read the SDDS data.

        This method reads only the last `lastrows` rows from each page of the SDDS file.
        """
        try:
            # Open SDDS file
            if sddsdata.InitializeInput(self.index, input) != 1:
                time.sleep(1)
                if sddsdata.InitializeInput(self.index, input) != 1:
                    raise ValueError("Failed to initialize SDDS input.")

            # Get data storage mode (SDDS_ASCII or SDDS_BINARY)
            self.mode = sddsdata.GetMode(self.index)

            # Get description text and contents
            self.description = sddsdata.GetDescription(self.index)

            # Get parameter names
            self.parameterName = sddsdata.GetParameterNames(self.index)
            numberOfParameters = len(self.parameterName)

            # Get array names
            self.arrayName = sddsdata.GetArrayNames(self.index)
            numberOfArrays = len(self.arrayName)

            # Get column names
            self.columnName = sddsdata.GetColumnNames(self.index)
            numberOfColumns = len(self.columnName)

            # Get parameter definitions
            self.parameterDefinition = [sddsdata.GetParameterDefinition(self.index, name) for name in self.parameterName]

            # Get array definitions
            self.arrayDefinition = [sddsdata.GetArrayDefinition(self.index, name) for name in self.arrayName]

            # Get column definitions
            self.columnDefinition = [sddsdata.GetColumnDefinition(self.index, name) for name in self.columnName]

            # Initialize parameter, array, and column data
            self.parameterData = [[] for _ in range(numberOfParameters)]
            self.arrayData = [[] for _ in range(numberOfArrays)]
            self.columnData = [[] for _ in range(numberOfColumns)]
            self.arrayDimensions = [[] for _ in range(numberOfArrays)]

            # Read in SDDS data (last rows)
            page = sddsdata.ReadPageLastRows(self.index, lastrows)
            if page != 1:
                raise Exception("Unable to read SDDS data for the first page")
            while page > 0:
                for i in range(numberOfParameters):
                    self.parameterData[i].append(sddsdata.GetParameter(self.index, i))
                for i in range(numberOfArrays):
                    self.arrayData[i].append(sddsdata.GetArray(self.index, i))
                    self.arrayDimensions[i].append(sddsdata.GetArrayDimensions(self.index, i))
                rows = sddsdata.RowCount(self.index)
                if rows > 0:
                    for i in range(numberOfColumns):
                        self.columnData[i].append(sddsdata.GetColumn(self.index, i))
                else:
                    for i in range(numberOfColumns):
                        self.columnData[i].append([])
                page = sddsdata.ReadPageLastRows(self.index, lastrows)

            # Close SDDS file
            if sddsdata.Terminate(self.index) != 1:
                raise ValueError("Failed to terminate SDDS input.")

        except:
            sddsdata.PrintErrors(self.SDDS_VERBOSE_PrintErrors)
            raise

    def save(self, output):
        """
        Saves the SDDS object's data to an SDDS file.

        Args:
            output (str): The output SDDS filename to save the data.

        Raises:
            Exception: If unable to write the SDDS data.

        This method writes the data stored in the SDDS object to the specified file, including the
        parameters, arrays, columns, and their data.
        """
        try:
            # Check for invalid SDDS data
            numberOfParameters = len(self.parameterName)
            numberOfArrays = len(self.arrayName)
            numberOfColumns = len(self.columnName)
            pages = 0

            if numberOfParameters != len(self.parameterData):
                raise Exception("Unmatched parameterName and parameterData")
            if numberOfArrays != len(self.arrayData):
                raise Exception("Unmatched arrayName and arrayData")
            if numberOfColumns != len(self.columnData):
                raise Exception("Unmatched columnName and columnData")
            if numberOfParameters != len(self.parameterDefinition):
                raise Exception("Unmatched parameterName and parameterDefinition")
            if numberOfArrays != len(self.arrayDefinition):
                raise Exception("Unmatched arrayName and arrayDefinition")
            if numberOfColumns != len(self.columnDefinition):
                raise Exception("Unmatched columnName and columnDefinition")

            if numberOfParameters > 0:
                pages = len(self.parameterData[0])
            elif numberOfColumns > 0:
                pages = len(self.columnData[0])
            elif numberOfArrays > 0:
                pages = len(self.arrayData[0])

            for i in range(numberOfParameters):
                if pages != len(self.parameterData[i]):
                    raise Exception("Unequal number of pages in parameter data")
            for i in range(numberOfArrays):
                if pages != len(self.arrayData[i]):
                    raise Exception("Unequal number of pages in array data")
                if pages != len(self.arrayDimensions[i]):
                    raise Exception("Unequal number of pages in array dimension data")
            for i in range(numberOfColumns):
                if pages != len(self.columnData[i]):
                    raise Exception("Unequal number of pages in column data")
            for page in range(pages):
                rows = 0
                if numberOfColumns > 0:
                    rows = len(self.columnData[0][page])
                for i in range(numberOfColumns):
                    if rows != len(self.columnData[i][page]):
                        raise Exception("Unequal number of rows in column data")

            # Open SDDS output file
            if sddsdata.InitializeOutput(self.index, self.mode, 1, self.description[0], self.description[1], output) != 1:
                raise ValueError("Failed to initialize SDDS output.")

            # Define parameters, arrays, and columns
            for i in range(numberOfParameters):
                if sddsdata.DefineParameter(
                    self.index,
                    self.parameterName[i],
                    self.parameterDefinition[i][0],
                    self.parameterDefinition[i][1],
                    self.parameterDefinition[i][2],
                    self.parameterDefinition[i][3],
                    self.parameterDefinition[i][4],
                    self.parameterDefinition[i][5],
                ) == -1:
                    raise ValueError("Failed to define parameter.")

            for i in range(numberOfArrays):
                if sddsdata.DefineArray(
                    self.index,
                    self.arrayName[i],
                    self.arrayDefinition[i][0],
                    self.arrayDefinition[i][1],
                    self.arrayDefinition[i][2],
                    self.arrayDefinition[i][3],
                    self.arrayDefinition[i][4],
                    self.arrayDefinition[i][5],
                    self.arrayDefinition[i][6],
                    self.arrayDefinition[i][7],
                ) == -1:
                    raise ValueError("Failed to define array.")

            for i in range(numberOfColumns):
                if sddsdata.DefineColumn(
                    self.index,
                    self.columnName[i],
                    self.columnDefinition[i][0],
                    self.columnDefinition[i][1],
                    self.columnDefinition[i][2],
                    self.columnDefinition[i][3],
                    self.columnDefinition[i][4],
                    self.columnDefinition[i][5],
                ) == -1:
                    raise ValueError("Failed to define column.")

            # Write SDDS header
            if sddsdata.WriteLayout(self.index) != 1:
                raise ValueError("Failed to write SDDS layout.")

            # Write SDDS data
            for page in range(pages):
                rows = 0
                if numberOfColumns > 0:
                    rows = len(self.columnData[0][page])
                if sddsdata.StartPage(self.index, rows) != 1:
                    raise ValueError("Failed to start SDDS page.")
                for i in range(numberOfParameters):
                    if sddsdata.SetParameter(self.index, i, self.parameterData[i][page]) != 1:
                        raise ValueError("Failed to set parameter value.")
                for i in range(numberOfArrays):
                    if sddsdata.SetArray(
                        self.index, i, self.arrayData[i][page], self.arrayDimensions[i][page]
                    ) != 1:
                        raise ValueError("Failed to set array value.")
                for i in range(numberOfColumns):
                    if sddsdata.SetColumn(self.index, i, self.columnData[i][page]) != 1:
                        raise ValueError("Failed to set column value.")
                if sddsdata.WritePage(self.index) != 1:
                    raise ValueError("Failed to write SDDS page.")

            # Close SDDS output file
            if sddsdata.Terminate(self.index) != 1:
                raise ValueError("Failed to terminate SDDS output.")
        except:
            sddsdata.PrintErrors(self.SDDS_VERBOSE_PrintErrors)
            raise

    def setDescription(self, text, contents):
        """
        Sets the description for the SDDS object.

        Args:
            text (str): Description text.
            contents (str): Description contents.
        """
        self.description = [text, contents]

    def defineParameter(self, name, symbol="", units="", description="", formatString="", type=SDDS_DOUBLE, fixedValue=""):
        """
        Defines a parameter for the SDDS object.

        Args:
            name (str): Parameter name.
            symbol (str, optional): Parameter symbol.
            units (str, optional): Parameter units.
            description (str, optional): Parameter description.
            formatString (str, optional): Format string for the parameter.
            type (int, optional): Data type of the parameter. Defaults to SDDS_DOUBLE. Valid options include:
                - SDDS_SHORT: 16-bit signed integer.
                - SDDS_USHORT: 16-bit unsigned integer.
                - SDDS_LONG: 32-bit signed integer.
                - SDDS_ULONG: 32-bit unsigned integer.
                - SDDS_LONG64: 64-bit signed integer.
                - SDDS_ULONG64: 64-bit unsigned integer.
                - SDDS_FLOAT: Single-precision floating-point number.
                - SDDS_DOUBLE: Double-precision floating-point number.
                - SDDS_LONGDOUBLE: Long double-precision floating-point number.
                - SDDS_STRING: String (textual data).
                - SDDS_CHARACTER: Single character.
            fixedValue (str, optional): Fixed value of the parameter.

        This method adds a parameter definition to the SDDS object.
        """
        self.parameterName.append(name)
        self.parameterDefinition.append(
            [symbol, units, description, formatString, type, fixedValue]
        )
        self.parameterData.append([])

    def defineSimpleParameter(self, name, type):
        """
        Defines a simple parameter with minimal information.

        Args:
            name (str): Parameter name.
            type (int): Data type of the parameter. Valid options include:
                - SDDS_SHORT: 16-bit signed integer.
                - SDDS_USHORT: 16-bit unsigned integer.
                - SDDS_LONG: 32-bit signed integer.
                - SDDS_ULONG: 32-bit unsigned integer.
                - SDDS_LONG64: 64-bit signed integer.
                - SDDS_ULONG64: 64-bit unsigned integer.
                - SDDS_FLOAT: Single-precision floating-point number.
                - SDDS_DOUBLE: Double-precision floating-point number.
                - SDDS_LONGDOUBLE: Long double-precision floating-point number.
                - SDDS_STRING: String (textual data).
                - SDDS_CHARACTER: Single character.

        This method adds a parameter definition with default attributes.
        """
        self.parameterName.append(name)
        self.parameterDefinition.append(["", "", "", "", type, ""])
        self.parameterData.append([])

    def defineArray(self, name, symbol="", units="", description="", formatString="", group_name="", type=SDDS_DOUBLE, fieldLength=0, dimensions=1):
        """
        Defines an array for the SDDS object.

        Args:
            name (str): Array name.
            symbol (str, optional): Array symbol.
            units (str, optional): Array units.
            description (str, optional): Array description.
            formatString (str, optional): Format string for the array.
            group_name (str, optional): Group name for the array.
            type (int, optional): Data type of the array. Defaults to SDDS_DOUBLE, Valid options include:
                - SDDS_SHORT: 16-bit signed integer.
                - SDDS_USHORT: 16-bit unsigned integer.
                - SDDS_LONG: 32-bit signed integer.
                - SDDS_ULONG: 32-bit unsigned integer.
                - SDDS_LONG64: 64-bit signed integer.
                - SDDS_ULONG64: 64-bit unsigned integer.
                - SDDS_FLOAT: Single-precision floating-point number.
                - SDDS_DOUBLE: Double-precision floating-point number.
                - SDDS_LONGDOUBLE: Long double-precision floating-point number.
                - SDDS_STRING: String (textual data).
                - SDDS_CHARACTER: Single character.
            fieldLength (int, optional): Field length for the array.
            dimensions (int, optional): Number of dimensions of the array. Defaults to 1

        This method adds an array definition to the SDDS object.
        """
        self.arrayName.append(name)
        self.arrayDefinition.append(
            [symbol, units, description, formatString, group_name, type, fieldLength, dimensions]
        )
        self.arrayData.append([])
        self.arrayDimensions.append([])

    def defineSimpleArray(self, name, type, dimensions):
        """
        Defines a simple array with minimal information.

        Args:
            name (str): Array name.
            type (int): Data type of the array. Valid options include:
                - SDDS_SHORT: 16-bit signed integer.
                - SDDS_USHORT: 16-bit unsigned integer.
                - SDDS_LONG: 32-bit signed integer.
                - SDDS_ULONG: 32-bit unsigned integer.
                - SDDS_LONG64: 64-bit signed integer.
                - SDDS_ULONG64: 64-bit unsigned integer.
                - SDDS_FLOAT: Single-precision floating-point number.
                - SDDS_DOUBLE: Double-precision floating-point number.
                - SDDS_LONGDOUBLE: Long double-precision floating-point number.
                - SDDS_STRING: String (textual data).
                - SDDS_CHARACTER: Single character.
            dimensions (int): Number of dimensions of the array.

        This method adds an array definition with default attributes.
        """
        self.arrayName.append(name)
        self.arrayDefinition.append(["", "", "", "", "", type, 0, dimensions])
        self.arrayData.append([])
        self.arrayDimensions.append([])

    def defineColumn(self, name, symbol="", units="", description="", formatString="", type=SDDS_DOUBLE, fieldLength=0):
        """
        Defines a column for the SDDS object.

        Args:
            name (str): Column name.
            symbol (str, optional): Column symbol.
            units (str, optional): Column units.
            description (str, optional): Column description.
            formatString (str, optional): Format string for the column.
            type (int, optional): Data type of the column. Defaults to SDDS_DOUBLE. Valid options include:
                - SDDS_SHORT: 16-bit signed integer.
                - SDDS_USHORT: 16-bit unsigned integer.
                - SDDS_LONG: 32-bit signed integer.
                - SDDS_ULONG: 32-bit unsigned integer.
                - SDDS_LONG64: 64-bit signed integer.
                - SDDS_ULONG64: 64-bit unsigned integer.
                - SDDS_FLOAT: Single-precision floating-point number.
                - SDDS_DOUBLE: Double-precision floating-point number.
                - SDDS_LONGDOUBLE: Long double-precision floating-point number.
                - SDDS_STRING: String (textual data).
                - SDDS_CHARACTER: Single character.
            fieldLength (int, optional): Field length for the column.

        This method adds a column definition to the SDDS object.
        """
        self.columnName.append(name)
        self.columnDefinition.append(
            [symbol, units, description, formatString, type, fieldLength]
        )
        self.columnData.append([])

    def defineSimpleColumn(self, name, type):
        """
        Defines a simple column with minimal information.

        Args:
            name (str): Column name.
            type (int): Data type of the column. Valid options include:
                - SDDS_SHORT: 16-bit signed integer.
                - SDDS_USHORT: 16-bit unsigned integer.
                - SDDS_LONG: 32-bit signed integer.
                - SDDS_ULONG: 32-bit unsigned integer.
                - SDDS_LONG64: 64-bit signed integer.
                - SDDS_ULONG64: 64-bit unsigned integer.
                - SDDS_FLOAT: Single-precision floating-point number.
                - SDDS_DOUBLE: Double-precision floating-point number.
                - SDDS_LONGDOUBLE: Long double-precision floating-point number.
                - SDDS_STRING: String (textual data).
                - SDDS_CHARACTER: Single character.

        This method adds a column definition with default attributes.
        """
        self.columnName.append(name)
        self.columnDefinition.append(["", "", "", "", type, 0])
        self.columnData.append([])

    def setParameterValueList(self, name, valueList):
        """
        Sets the list of parameter values for a parameter. This can be used to set values for multiple pages at once.

        Args:
            name (str): Parameter name.
            valueList (list): List of values for the parameter.

        Raises:
            Exception: If the parameter name is invalid.

        This method assigns a list of values to a parameter across pages.
        """
        numberOfParameters = len(self.parameterName)
        if name in self.parameterName:
            self.parameterData[self.parameterName.index(name)] = valueList
        else:
            msg = "Invalid parameter name " + name
            raise Exception(msg)

    def getParameterValueList(self, name):
        """
        Gets the list of parameter values for a parameter. This can be used to get values for multiple pages at once.

        Args:
            name (str): Parameter name.

        Returns:
            list: List of values for the parameter.
            
        Raises:
            Exception: If the parameter name is invalid.

        """
        numberOfParameters = len(self.parameterName)
        if name in self.parameterName:
            return(self.parameterData[self.parameterName.index(name)])
        else:
            msg = "Invalid parameter name " + name
            raise Exception(msg)

    def setParameterValue(self, name, value, page=1):
        """
        Sets a single parameter value at a specific page.

        Args:
            name (str): Parameter name.
            value: Parameter value.
            page (int, optional): Page number (1-based index, defaults to 1).

        Raises:
            Exception: If the parameter name or page is invalid.

        This method sets the parameter value at the specified page.
        """
        page = page - 1
        numberOfParameters = len(self.parameterName)
        if name in self.parameterName:
            i = self.parameterName.index(name)
            if len(self.parameterData[i]) == page:
                self.parameterData[i][page:] = [value]
            elif len(self.parameterData[i]) < page or page < 0:
                msg = "Invalid page " + str(page + 1)
                raise Exception(msg)
            else:
                self.parameterData[i][page] = value
        else:
            msg = "Invalid parameter name " + name
            raise Exception(msg)

    def getParameterValue(self, name, page=1):
        """
        Gets a single parameter value at a specific page.

        Args:
            name (str): Parameter name.
            page (int, optional): Page number (1-based index, defaults to 1).

        Returns:
            value: Parameter value.
            
        Raises:
            Exception: If the parameter name or page is invalid.
        """
        page = page - 1
        numberOfParameters = len(self.parameterName)
        if name in self.parameterName:
            i = self.parameterName.index(name)
            if len(self.parameterData[i]) == page:
                return(self.parameterData[i][page:])
            elif len(self.parameterData[i]) < page or page < 0:
                msg = "Invalid page " + str(page + 1)
                raise Exception(msg)
            else:
                return(self.parameterData[i][page])
        else:
            msg = "Invalid parameter name " + name
            raise Exception(msg)

    def setArrayValueLists(self, name, valueList, dimensionList):
        """
        Sets the nested list of array values and dimensions for a single array. This can be used to set values for multiple pages at once.

        Args:
            name (str): Array name.
            valueList (list): Nested list of array values.
            dimensionList (list): Nested list of array dimensions.

        Raises:
            Exception: If the array name is invalid.

        This method assigns values and dimensions to an array across pages.
        """
        numberOfArrays = len(self.arrayName)
        if name in self.arrayName:
            i = self.arrayName.index(name)
            self.arrayDimensions[i] = dimensionList
            self.arrayData[i] = valueList
        else:
            msg = "Invalid array name " + name
            raise Exception(msg)

    def getArrayValueLists(self, name):
        """
        Gets the nested list of array values for a single array. This can be used to get values for multiple pages at once.

        Args:
            name (str): Array name.

        Returns:
            list: Nested list of array values.
            
        Raises:
            Exception: If the array name is invalid.
        """
        numberOfArrays = len(self.arrayName)
        if name in self.arrayName:
            return(self.arrayData[self.arrayName.index(name)])
        else:
            msg = "Invalid array name " + name
            raise Exception(msg)

    def getArrayDimensionLists(self, name):
        """
        Gets the nested list of array dimensions for a single array. This can be used to get dimensions for multiple pages at once.

        Args:
            name (str): Array name.

        Returns:
            list: Nested list of array dimensions.
            
        Raises:
            Exception: If the array name is invalid.
        """
        numberOfArrays = len(self.arrayName)
        if name in self.arrayName:
            return(self.arrayDimensions[self.arrayName.index(name)])
        else:
            msg = "Invalid array name " + name
            raise Exception(msg)

    def setArrayValueList(self, name, valueList, dimensionList, page=1):
        """
        Sets a single array value and dimension at a specific page.

        Args:
            name (str): Array name.
            valueList (list): Array values.
            dimensionList (list): Array dimensions.
            page (int, optional): Page number (1-based index, defaults to 1).

        Raises:
            Exception: If the array name or page is invalid.

        This method sets the array values and dimensions at the specified page.
        """
        page = page - 1
        numberOfArrays = len(self.arrayName)
        if name in self.arrayName:
            i = self.arrayName.index(name)
            if len(self.arrayData[i]) == page:
                self.arrayData[i][page:] = [valueList]
                self.arrayDimensions[i][page:] = [dimensionList]
            elif len(self.arrayData[i]) < page or page < 0:
                msg = "Invalid page " + str(page + 1)
                raise Exception(msg)
            else:
                self.arrayData[i][page] = valueList
                self.arrayDimensions[i][page] = dimensionList
        else:
            msg = "Invalid array name " + name
            raise Exception(msg)

    def getArrayValueList(self, name, page=1):
        """
        Gets a single array value at a specific page.

        Args:
            name (str): Array name.
            page (int, optional): Page number (1-based index, defaults to 1).

        Returns:
            list: Array values.
            
        Raises:
            Exception: If the array name or page is invalid.
        """
        page = page - 1
        numberOfArrays = len(self.arrayName)
        if name in self.arrayName:
            i = self.arrayName.index(name)
            if len(self.arrayData[i]) == page:
                return(self.arrayData[i][page:])
            elif len(self.arrayData[i]) < page or page < 0:
                msg = "Invalid page " + str(page + 1)
                raise Exception(msg)
            else:
                return(self.arrayData[i][page])
        else:
            msg = "Invalid array name " + name
            raise Exception(msg)

    def getArrayDimensionList(self, name, page=1):
        """
        Gets a single array dimension at a specific page.

        Args:
            name (str): Array name.
            page (int, optional): Page number (1-based index, defaults to 1).

        Returns:
            list: Array dimension.
            
        Raises:
            Exception: If the array name or page is invalid.
        """
        page = page - 1
        numberOfArrays = len(self.arrayName)
        if name in self.arrayName:
            i = self.arrayName.index(name)
            if len(self.arrayData[i]) == page:
                return(self.arrayDimensions[i][page:])
            elif len(self.arrayData[i]) < page or page < 0:
                msg = "Invalid page " + str(page + 1)
                raise Exception(msg)
            else:
                return(self.arrayDimensions[i][page])
        else:
            msg = "Invalid array name " + name
            raise Exception(msg)

    def setColumnValueLists(self, name, valueList):
        """
        Sets the nested list of column values for a single column. This can be used to set values for multiple pages at once.

        Args:
            name (str): Column name.
            valueList (list): Nested list of column values.

        Raises:
            Exception: If the column name is invalid.

        This method assigns a list of values to a column across pages.
        """
        numberOfColumns = len(self.columnName)
        if name in self.columnName:
            self.columnData[self.columnName.index(name)] = valueList
        else:
            msg = "Invalid column name " + name
            raise Exception(msg)

    def getColumnValueLists(self, name):
        """
        Gets the nested list of column values for a single column. This can be used to get values for multiple pages at once.

        Args:
            name (str): Column name.

        Returns:
            list: Nested list of column values.

        Raises:
            Exception: If the column name is invalid.
        """
        numberOfColumns = len(self.columnName)
        if name in self.columnName:
            return(self.columnData[self.columnName.index(name)])
        else:
            msg = "Invalid column name " + name
            raise Exception(msg)

    def setColumnValueList(self, name, valueList, page=1):
        """
        Sets a single column value list at a specific page.

        Args:
            name (str): Column name.
            valueList (list): Column values.
            page (int, optional): Page number (1-based index, defaults to 1).

        Raises:
            Exception: If the column name or page is invalid.

        This method sets the column values at the specified page.
        """
        page = page - 1
        if name in self.columnName:
            i = self.columnName.index(name)
            if len(self.columnData[i]) == page:
                self.columnData[i][page:] = [valueList]
            elif len(self.columnData[i]) < page or page < 0:
                msg = "Invalid page " + str(page + 1)
                raise Exception(msg)
            else:
                self.columnData[i][page] = valueList
        else:
            msg = "Invalid column name " + name
            raise Exception(msg)

    def getColumnValueList(self, name, page=1):
        """
        Gets a single column value list at a specific page.

        Args:
            name (str): Column name.
            page (int, optional): Page number (1-based index, defaults to 1).

        Returns:
            list: Column values.
            
        Raises:
            Exception: If the column name or page is invalid.
        """
        page = page - 1
        numberOfColumns = len(self.columnName)
        if name in self.columnName:
            i = self.columnName.index(name)
            if len(self.columnData[i]) == page:
                return(self.columnData[i][page:])
            elif len(self.columnData[i]) < page or page < 0:
                msg = "Invalid page " + str(page + 1)
                raise Exception(msg)
            else:
                return(self.columnData[i][page])
        else:
            msg = "Invalid column name " + name
            raise Exception(msg)

    def setColumnValue(self, name, value, page=1, row=1):
        """
        Sets a single column value at a specific page and row.

        Args:
            name (str): Column name.
            value: Column value.
            page (int, optional): Page number (1-based index, defaults to 1).
            row (int, optional): Row number (1-based index, defaults to 1).

        Raises:
            Exception: If the column name, page, or row is invalid.

        This method sets the column value at the specified page and row.
        """
        page = page - 1
        row = row - 1
        numberOfColumns = len(self.columnName)
        if name in self.columnName:
            i = self.columnName.index(name)
            if len(self.columnData[i]) == page:
                if row == 0:
                    self.columnData[i][page:] = [[value]]
                else:
                    msg = "Invalid row " + str(row + 1)
                    raise Exception(msg)
            elif len(self.columnData[i]) < page or page < 0:
                msg = "Invalid page " + str(page + 1)
                raise Exception(msg)
            else:
                if len(self.columnData[i][page]) == row:
                    self.columnData[i][page][row:] = [value]
                elif len(self.columnData[i][page]) < row or row < 0:
                    msg = "Invalid row " + str(row + 1)
                    raise Exception(msg)
                else:
                    self.columnData[i][page][row] = value
        else:
            msg = "Invalid column name " + name
            raise Exception(msg)

    def getColumnValue(self, name, page=1, row=1):
        """
        Gets a single column value at a specific page and row.

        Args:
            name (str): Column name.
            page (int, optional): Page number (1-based index, defaults to 1).
            row (int, optional): Row number (1-based index, defaults to 1).

        Returns:
            value: Column value.
            
        Raises:
            Exception: If the column name, page, or row is invalid.
        """
        page = page - 1
        row = row - 1
        numberOfColumns = len(self.columnName)
        if name in self.columnName:
            i = self.columnName.index(name)
            if len(self.columnData[i]) == page:
                if row == 0:
                    return(self.columnData[i][page:])
                else:
                    msg = "Invalid row " + str(row + 1)
                    raise Exception(msg)
            elif len(self.columnData[i]) < page or page < 0:
                msg = "Invalid page " + str(page + 1)
                raise Exception(msg)
            else:
                if len(self.columnData[i][page]) == row:
                    return(self.columnData[i][page][row:])
                elif len(self.columnData[i][page]) < row or row < 0:
                    msg = "Invalid row " + str(row + 1)
                    raise Exception(msg)
                else:
                    return(self.columnData[i][page][row])
        else:
            msg = "Invalid column name " + name
            raise Exception(msg)

    def getParameterCount(self):
        """
        Retrieves the number of parameters.

        Returns:
            int: The number of parameters.
        """
        return(len(self.parameterName))

    def getArrayCount(self):
        """
        Retrieves the number of arrays.

        Returns:
            int: The number of arrays.
        """
        return(len(self.arrayName))

    def getColumnCount(self):
        """
        Retrieves the number of columns.

        Returns:
            int: The number of columns.
        """
        return(len(self.columnName))

    def getParameterNames(self):
        """
        Retrieves a list of all parameter names.

        Returns:
            list: A list of parameter names as strings, or an empty list.
        """
        return(self.parameterName)

    def getArrayNames(self):
        """
        Retrieves a list of all array names.

        Returns:
            list: A list of array names as strings, or an empty list.
        """
        return(self.arrayName)

    def getColumnNames(self):
        """
        Retrieves a list of all column names.

        Returns:
            list: A list of column names as strings, or an empty list.
        """
        return(self.columnName)

    def getParameterDatatype(self, name):
        """
        Retrieves the data type of a parameter.

        Args:
            name (str): Parameter name.
        
        Returns:
            int: Number representing the data type of the parameter.
        """
        if name in self.parameterName:
            return(self.parameterDefinition[self.parameterName.index(name)][4])
        else:
            msg = "Invalid parameter name " + name
            raise Exception(msg)

    def getArrayDatatype(self, name):
        """
        Retrieves the data type of a array.

        Args:
            name (str): Array name.
        
        Returns:
            int: Number representing the data type of the array.
        """
        if name in self.arrayName:
            return(self.arrayDefinition[self.arrayName.index(name)][5])
        else:
            msg = "Invalid array name " + name
            raise Exception(msg)
    
    def getColumnDatatype(self, name):
        """
        Retrieves the data type of a column.

        Args:
            name (str): Column name.
        
        Returns:
            int: Number representing the data type of the column.
        """
        if name in self.columnName:
            return(self.columnDefinition[self.columnName.index(name)][4])
        else:
            msg = "Invalid column name " + name
            raise Exception(msg)

    def getParameterUnits(self, name):
        """
        Retrieves the units of a parameter.

        Args:
            name (str): Parameter name.
        
        Returns:
            str: Units of the parameter.
        """
        if name in self.parameterName:
            return(self.parameterDefinition[self.parameterName.index(name)][1])
        else:
            msg = "Invalid parameter name " + name
            raise Exception(msg)

    def getArrayDatatype(self, name):
        """
        Retrieves the units of a array.
    
        Args:
            name (str): Array name.
        
        Returns:
            str: Units of the array.
        """
        if name in self.arrayName:
            return(self.arrayDefinition[self.arrayName.index(name)][1])
        else:
            msg = "Invalid array name " + name
            raise Exception(msg)
    
    def getColumnDatatype(self, name):
        """
        Retrieves the units of a column.

        Args:
            name (str): Column name.
        
        Returns:
            str: Units of the column.
        """
        if name in self.columnName:
            return(self.columnDefinition[self.columnName.index(name)][1])
        else:
            msg = "Invalid column name " + name
            raise Exception(msg)

    def pageCount(self):
        """
        Retrieves the number or loaded pages

        Returns:
            int: Number or loaded pages
        """
        return(self.loaded_pages)



# Expose constants to the module-level namespace
SDDS_VERBOSE_PrintErrors = SDDS.SDDS_VERBOSE_PrintErrors
SDDS_EXIT_PrintErrors = SDDS.SDDS_EXIT_PrintErrors
SDDS_CHECK_OKAY = SDDS.SDDS_CHECK_OKAY
SDDS_CHECK_NONEXISTENT = SDDS.SDDS_CHECK_NONEXISTENT
SDDS_CHECK_WRONGTYPE = SDDS.SDDS_CHECK_WRONGTYPE
SDDS_CHECK_WRONGUNITS = SDDS.SDDS_CHECK_WRONGUNITS
SDDS_LONGDOUBLE = SDDS.SDDS_LONGDOUBLE
SDDS_DOUBLE = SDDS.SDDS_DOUBLE
SDDS_REAL64 = SDDS.SDDS_REAL64
SDDS_FLOAT = SDDS.SDDS_FLOAT
SDDS_REAL32 = SDDS.SDDS_REAL32
SDDS_LONG64 = SDDS.SDDS_LONG64
SDDS_INT64 = SDDS.SDDS_INT64 
SDDS_ULONG64 = SDDS.SDDS_ULONG64
SDDS_UINT64 = SDDS.SDDS_UINT64
SDDS_LONG = SDDS.SDDS_LONG
SDDS_INT32 = SDDS.SDDS_INT32
SDDS_ULONG = SDDS.SDDS_ULONG
SDDS_UINT32 = SDDS.SDDS_UINT32
SDDS_SHORT = SDDS.SDDS_SHORT
SDDS_INT16 = SDDS.SDDS_INT16
SDDS_USHORT = SDDS.SDDS_USHORT
SDDS_UINT16 = SDDS.SDDS_UINT16
SDDS_STRING = SDDS.SDDS_STRING
SDDS_CHARACTER = SDDS.SDDS_CHARACTER
SDDS_NUM_TYPES = SDDS.SDDS_NUM_TYPES
SDDS_BINARY = SDDS.SDDS_BINARY 
SDDS_ASCII = SDDS.SDDS_ASCII
SDDS_FLUSH_TABLE = SDDS.SDDS_FLUSH_TABLE

def load(input_file) -> SDDS:
    """
    Loads an SDDS file into the SDDS object.

    Args:
        input_file (str): The input SDDS filename to load.

    Raises:
        Exception: If unable to read the SDDS data.

    This method reads the SDDS file specified by `input`, and populates the object's data structures
    with the parameters, arrays, columns, and their respective data.
    """

    # Initialize an SDDS object
    sdds_obj = SDDS()

    # Load the file
    try:
        sdds_obj.load(input_file)
    except Exception as e:
        raise ValueError(f"Failed to load the SDDS file: {e}")

    return sdds_obj

def loadSparse(input_file, interval, offset) -> SDDS:
    """
    Loads an SDDS file into the SDDS object with sparse data reading.

    Args:
        input_file (str): The input SDDS filename to load.
        interval (int): Interval between pages to read.
        offset (int): Offset to start reading from.

    Raises:
        Exception: If unable to read the SDDS data.

    This method reads every `interval` pages from the SDDS file, starting from the page at `offset`.
    """
    # Initialize an SDDS object
    sdds_obj = SDDS()

    # Load the file
    try:
        sdds_obj.loadSparse(input_file, interval, offset)
    except Exception as e:
        raise ValueError(f"Failed to load the SDDS file: {e}")

    return sdds_obj

def loadLastRows(input_file, lastrows) -> SDDS:
    """
    Loads an SDDS file into the SDDS object, reading only the last few rows.

    Args:
        input_file (str): The input SDDS filename to load.
        lastrows (int): Number of last rows to read.

    Raises:
        Exception: If unable to read the SDDS data.

    This method reads only the last `lastrows` rows from each page of the SDDS file.
    """
    # Initialize an SDDS object
    sdds_obj = SDDS()

    # Load the file
    try:
        sdds_obj.loadLastRows(input_file, lastrows)
    except Exception as e:
        raise ValueError(f"Failed to load the SDDS file: {e}")

    return sdds_obj

def save(sdds_obj : SDDS, output_file):
    """
    Saves the SDDS object's data to an SDDS file.

    Args:
        output (str): The output SDDS filename to save the data.

    Raises:
        Exception: If unable to write the SDDS data.

    This method writes the data stored in the SDDS object to the specified file, including the
    parameters, arrays, columns, and their data.
    """
    # Save the file
    try:
        sdds_obj.save(output_file)
    except Exception as e:
        raise ValueError(f"Failed to load the SDDS file: {e}")

def sdds_data_type_to_string(data_type_code):
    """
    Converts a numeric SDDS data type code to its string representation.
    
    Args:
        data_type_code (int): Numeric code of the SDDS data type.
        
    Returns:
        str: String representation of the SDDS data type.
    """
    # Mapping of SDDS numeric data type codes to string representations
    data_type_map = {
        1: "SDDS_LONGDOUBLE",
        2: "SDDS_DOUBLE",
        3: "SDDS_FLOAT",
        4: "SDDS_LONG64",
        5: "SDDS_ULONG64",
        6: "SDDS_LONG",
        7: "SDDS_ULONG",
        8: "SDDS_SHORT",
        9: "SDDS_USHORT",
        10: "SDDS_STRING",
        11: "SDDS_CHARACTER",
    }
    return data_type_map.get(data_type_code, "Unknown Data Type")

def sdds_data_type_to_short_string(data_type_code):
    """
    Converts a numeric SDDS data type code to its short string representation.
    
    Args:
        data_type_code (int): Numeric code of the SDDS data type.
        
    Returns:
        str: String representation of the SDDS data type.
    """
    # Mapping of SDDS numeric data type codes to string representations
    data_type_map = {
        1: "longdouble",
        2: "double",
        3: "float",
        4: "long64",
        5: "ulong64",
        6: "long",
        7: "ulong",
        8: "short",
        9: "ushort",
        10: "string",
        11: "character",
    }
    return data_type_map.get(data_type_code, "Unknown Data Type")

def sdds_short_string_to_data_type(data_type_code):
    """
    Converts a numeric SDDS data type code to its short string representation.
    
    Args:
        data_type_code (string): String representation of the SDDS data type.
        
    Returns:
        int: Numeric code of the SDDS data type.
    """
    # Mapping of SDDS numeric data type codes to string representations
    data_type_map = {
        "longdouble": 1,
        "double": 2,
        "float": 3,
        "long64": 4,
        "ulong64": 5,
        "long": 6,
        "ulong": 7,
        "short": 8,
        "ushort": 9,
        "string": 10,
        "character": 11,
    }
    return data_type_map.get(data_type_code, "Unknown Data Type")

def demo1(output):
    """
    Demonstrates how to save a demo SDDS file using the SDDS class.

    Args:
        output (str): The output SDDS filename to save the demo data.

    This function creates an SDDS object, populates it with sample data, and saves it to the specified output file.
    """
    x = SDDS()
    x.description[0] = "text"
    x.description[1] = "contents"
    x.parameterName = ["ShortP", "LongP", "FloatP", "DoubleP", "StringP", "CharacterP"]
    x.parameterData = [[1, 6], [2, 7], [3.3, 8.8], [4.4, 9.8], ["five", "ten"], ["a", "b"]]
    x.parameterDefinition = [
        ["", "", "", "", x.SDDS_SHORT, ""],
        ["", "", "", "", x.SDDS_LONG, ""],
        ["", "", "", "", x.SDDS_FLOAT, ""],
        ["", "", "", "", x.SDDS_DOUBLE, ""],
        ["", "", "", "", x.SDDS_STRING, ""],
        ["", "", "", "", x.SDDS_CHARACTER, ""],
    ]

    x.arrayName = ["ShortA", "LongA", "FloatA", "DoubleA", "StringA", "CharacterA"]
    x.arrayDefinition = [
        ["", "", "", "", "", x.SDDS_SHORT, 0, 1],
        ["", "", "", "", "", x.SDDS_LONG, 0, 1],
        ["", "", "", "", "", x.SDDS_FLOAT, 0, 2],
        ["", "", "", "", "", x.SDDS_DOUBLE, 0, 1],
        ["", "", "", "", "", x.SDDS_STRING, 0, 1],
        ["", "", "", "", "", x.SDDS_CHARACTER, 0, 1],
    ]
    x.arrayDimensions = [
        [[6], [8]],
        [[5], [7]],
        [[2, 3], [2, 4]],
        [[4], [5]],
        [[4], [5]],
        [[4], [5]],
    ]
    x.arrayData = [
        [[1, 2, 3, 4, 5, 6], [1, 2, 3, 4, 5, 6, 7, 8]],
        [[1, 2, 3, 4, 5], [1, 2, 3, 4, 5, 6, 7]],
        [[1, 2, 3, 4, 5, 6], [1, 2, 3, 4, 5, 6, 7, 8]],
        [[1, 2, 3, 4], [1, 2, 3, 4, 5]],
        [["one", "two", "three", "four"], ["five", "six", "seven", "eight", "nine"]],
        [["a", "b", "c", "d"], ["e", "f", "g", "h", "i"]],
    ]

    x.columnName = ["ShortC", "LongC", "FloatC", "DoubleC", "StringC", "CharacterC"]
    x.columnData = [
        [[1, 2, 3], [-1, -2, -3, -4]],
        [[1, 2, 3], [-1, -2, -3, -4]],
        [[1, 2, 3], [-1, -2, -3.6, -4.4]],
        [[1, 2, 3], [-1, -2, -3.6, -4.4]],
        [["row 1", "row 2", "row 3"], ["row 1", "row 2", "row 3", "row 4"]],
        [["x", "y", "z"], ["i", "j", "k", "l"]],
    ]
    x.columnDefinition = [
        ["", "", "", "", x.SDDS_SHORT, 0],
        ["", "", "", "", x.SDDS_LONG, 0],
        ["", "", "", "", x.SDDS_FLOAT, 0],
        ["", "", "", "", x.SDDS_DOUBLE, 0],
        ["", "", "", "", x.SDDS_STRING, 0],
        ["", "", "", "", x.SDDS_CHARACTER, 0],
    ]

    x.save(output)
    del x


def demo2(output):
    """
    Demonstrates how to save a demo SDDS file using the SDDS class with simplified definitions.

    Args:
        output (str): The output SDDS filename to save the demo data.

    This function shows how to use simplified methods to define parameters, arrays, and columns.
    """
    x = SDDS()
    x.setDescription("text", "contents")
    names = ["Short", "Long", "Float", "Double", "String", "Character"]
    types = [x.SDDS_SHORT, x.SDDS_LONG, x.SDDS_FLOAT, x.SDDS_DOUBLE, x.SDDS_STRING, x.SDDS_CHARACTER]
    for i in range(6):
        x.defineSimpleParameter(names[i] + "P", types[i])
        if types[i] == x.SDDS_FLOAT:
            x.defineSimpleArray(names[i] + "A", types[i], 2)
        else:
            x.defineSimpleArray(names[i] + "A", types[i], 1)
        x.defineSimpleColumn(names[i] + "C", types[i])
    parameterData = [[1, 6], [2, 7], [3.3, 8.8], [4.4, 9.8], ["five", "ten"], ["a", "b"]]
    for i in range(6):
        x.setParameterValueList(names[i] + "P", parameterData[i])

    arrayDimensions = [
        [[6], [8]],
        [[5], [7]],
        [[2, 3], [2, 4]],
        [[4], [5]],
        [[4], [5]],
        [[4], [5]],
    ]
    arrayData = [
        [[1, 2, 3, 4, 5, 6], [1, 2, 3, 4, 5, 6, 7, 8]],
        [[1, 2, 3, 4, 5], [1, 2, 3, 4, 5, 6, 7]],
        [[1, 2, 3, 4, 5, 6], [1, 2, 3, 4, 5, 6, 7, 8]],
        [[1, 2, 3, 4], [1, 2, 3, 4, 5]],
        [["one", "two", "three", "four"], ["five", "six", "seven", "eight", "nine"]],
        [["a", "b", "c", "d"], ["e", "f", "g", "h", "i"]],
    ]
    for i in range(6):
        x.setArrayValueLists(names[i] + "A", arrayData[i], arrayDimensions[i])

    columnData = [
        [[1, 2, 3], [-1, -2, -3, -4]],
        [[1, 2, 3], [-1, -2, -3, -4]],
        [[1, 2, 3], [-1, -2, -3.6, -4.4]],
        [[1, 2, 3], [-1, -2, -3.6, -4.4]],
        [["row 1", "row 2", "row 3"], ["row 1", "row 2", "row 3", "row 4"]],
        [["x", "y", "z"], ["i", "j", "k", "l"]],
    ]
    for i in range(6):
        x.setColumnValueLists(names[i] + "C", columnData[i])
    x.save(output)


def demo3(output):
    """
    Demonstrates how to save a demo SDDS file using `sddsdata` commands directly.

    Args:
        output (str): The output SDDS filename to save the demo data.

    This function shows how to use `sddsdata` module functions directly to create and save an SDDS file.
    """
    x = SDDS()

    try:
        # Open SDDS output file
        if sddsdata.InitializeOutput(x.index, x.SDDS_BINARY, 1, "", "", output) != 1:
            raise ValueError("Failed to initialize SDDS output.")
        # Setting column_major to true. Only use this if you are going to write whole columns and not one row at a time.
        sddsdata.SetColumnMajorOrder(x.index)
        # Define parameters
        if sddsdata.DefineSimpleParameter(x.index, "ParameterA", "mm", x.SDDS_DOUBLE) != 1:
            raise ValueError("Failed to define parameter.")
        # Define arrays
        if sddsdata.DefineSimpleArray(x.index, "ArrayA", "DegC", x.SDDS_DOUBLE, 1) != 1:
            raise ValueError("Failed to define array.")
        if sddsdata.DefineSimpleArray(x.index, "ArrayB", "DegC", x.SDDS_DOUBLE, 2) != 1:
            raise ValueError("Failed to define array.")
        # Define columns
        if sddsdata.DefineSimpleColumn(x.index, "ColumnA", "Volts", x.SDDS_DOUBLE) != 1:
            raise ValueError("Failed to define column.")
        if sddsdata.DefineSimpleColumn(x.index, "ColumnB", "Amps", x.SDDS_DOUBLE) != 1:
            raise ValueError("Failed to define column.")
        # Write SDDS header
        if sddsdata.WriteLayout(x.index) != 1:
            raise ValueError("Failed to write SDDS layout.")
        # Start SDDS page. Allocate 100 rows.
        if sddsdata.StartPage(x.index, 100) != 1:
            raise ValueError("Failed to start SDDS page.")
        # Set parameter values
        if sddsdata.SetParameter(x.index, "ParameterA", 1.1) != 1:
            raise ValueError("Failed to set parameter value.")
        # Set array values
        if sddsdata.SetArray(x.index, "ArrayA", [1, 2, 3], [3]) != 1:
            raise ValueError("Failed to set array value.")
        if sddsdata.SetArray(x.index, "ArrayB", [1, 2, 3, 4, 5, 6], [2, 3]) != 1:
            raise ValueError("Failed to set array value.")
        # Set column values
        if sddsdata.SetColumn(x.index, "ColumnA", [1, 2, 3]) != 1:
            raise ValueError("Failed to set column value.")
        if sddsdata.SetColumn(x.index, "ColumnB", [1, 2, 3]) != 1:
            raise ValueError("Failed to set column value.")
        # Write page to disk
        if sddsdata.WritePage(x.index) != 1:
            raise ValueError("Failed to write SDDS page.")
        # Close SDDS output file
        if sddsdata.Terminate(x.index) != 1:
            raise ValueError("Failed to terminate SDDS output.")

    except:
        sddsdata.PrintErrors(x.SDDS_VERBOSE_PrintErrors)
        raise


def demo4(output):
    """
    Demonstrates how to save a demo SDDS file using `sddsdata` commands and writing one row at a time.

    Args:
        output (str): The output SDDS filename to save the demo data.

    This function shows how to write data to an SDDS file one row at a time, useful for logging applications.
    """
    x = SDDS()

    try:
        # Open SDDS output file
        if sddsdata.InitializeOutput(x.index, x.SDDS_BINARY, 1, "", "", output) != 1:
            raise ValueError("Failed to initialize SDDS output.")
        # Turning on fsync mode and fixed rows count mode. These are useful for loggers.
        sddsdata.EnableFSync(x.index)
        sddsdata.SetFixedRowCountMode(x.index)
        # Define parameters
        if sddsdata.DefineSimpleParameter(x.index, "ParameterA", "mm", x.SDDS_DOUBLE) != 1:
            raise ValueError("Failed to define parameter.")
        # Define arrays
        if sddsdata.DefineSimpleArray(x.index, "ArrayA", "DegC", x.SDDS_DOUBLE, 1) != 1:
            raise ValueError("Failed to define array.")
        if sddsdata.DefineSimpleArray(x.index, "ArrayB", "DegC", x.SDDS_DOUBLE, 2) != 1:
            raise ValueError("Failed to define array.")
        # Define columns
        if sddsdata.DefineSimpleColumn(x.index, "ColumnA", "Volts", x.SDDS_DOUBLE) != 1:
            raise ValueError("Failed to define column.")
        if sddsdata.DefineSimpleColumn(x.index, "ColumnB", "Amps", x.SDDS_DOUBLE) != 1:
            raise ValueError("Failed to define column.")
        # Write SDDS header
        if sddsdata.WriteLayout(x.index) != 1:
            raise ValueError("Failed to write SDDS layout.")
        # Start SDDS page, allocate 2 rows.
        if sddsdata.StartPage(x.index, 2) != 1:
            raise ValueError("Failed to start SDDS page.")
        # Set parameter values
        if sddsdata.SetParameter(x.index, "ParameterA", 1.1) != 1:
            raise ValueError("Failed to set parameter value.")
        # Set array values
        if sddsdata.SetArray(x.index, "ArrayA", [1, 2, 3], [3]) != 1:
            raise ValueError("Failed to set array value.")
        if sddsdata.SetArray(x.index, "ArrayB", [1, 2, 3, 4, 5, 6], [2, 3]) != 1:
            raise ValueError("Failed to set array value.")
        # Set all columns, one row at a time
        if sddsdata.SetRowValues(x.index, 0, ["ColumnA", 1, "ColumnB", 1]) != 1:
            raise ValueError("Failed to set row values.")
        if sddsdata.SetRowValues(x.index, 1, ["ColumnA", 2, "ColumnB", 2]) != 1:
            raise ValueError("Failed to set row values.")
        # Update page because we reached the row allocation limit set in the StartPage command
        if sddsdata.UpdatePage(x.index, x.SDDS_FLUSH_TABLE) != 1:
            raise ValueError("Failed to update SDDS page.")
        # Set more rows
        if sddsdata.SetRowValues(x.index, 2, ["ColumnA", 3, "ColumnB", 3]) != 1:
            raise ValueError("Failed to set row values.")
        # Update page
        if sddsdata.UpdatePage(x.index, x.SDDS_FLUSH_TABLE) != 1:
            raise ValueError("Failed to update SDDS page.")
        # Close SDDS output file
        if sddsdata.Terminate(x.index) != 1:
            raise ValueError("Failed to terminate SDDS output.")

    except:
        sddsdata.PrintErrors(x.SDDS_VERBOSE_PrintErrors)
        raise


def demo5(output):
    """
    Demonstrates how to open an existing SDDS file and add rows to the last page without loading the whole file into memory.

    Args:
        output (str): The output SDDS filename to append data.

    This function shows how to append data to an existing SDDS file efficiently.
    """
    x = SDDS()

    try:
        # Open SDDS output file
        rows = sddsdata.InitializeAppendToPage(x.index, output, 100)
        if rows == 0:
            raise ValueError("Failed to initialize appending to SDDS page.")
        # Set all columns, one row at a time
        if sddsdata.SetRowValues(x.index, rows, ["ColumnA", 4, "ColumnB", 4]) != 1:
            raise ValueError("Failed to set row values.")
        if sddsdata.SetRowValues(x.index, rows + 1, ["ColumnA", 5, "ColumnB", 5]) != 1:
            raise ValueError("Failed to set row values.")
        if sddsdata.SetRowValues(x.index, rows + 2, ["ColumnA", 6, "ColumnB", 6]) != 1:
            raise ValueError("Failed to set row values.")
        # Update page
        if sddsdata.UpdatePage(x.index, x.SDDS_FLUSH_TABLE) != 1:
            raise ValueError("Failed to update SDDS page.")
        # Close SDDS output file
        if sddsdata.Terminate(x.index) != 1:
            raise ValueError("Failed to terminate SDDS output.")

    except:
        sddsdata.PrintErrors(x.SDDS_VERBOSE_PrintErrors)
        raise


def demo6(output):
    """
    Demonstrates how to open an existing SDDS file and add a new page.

    Args:
        output (str): The output SDDS filename to append data.

    This function shows how to append a new page to an existing SDDS file.
    """
    x = SDDS()

    try:
        # Open SDDS output file
        if sddsdata.InitializeAppend(x.index, output) != 1:
            raise ValueError("Failed to initialize appending to SDDS file.")
        # Allocate rows
        if sddsdata.StartPage(x.index, 100) != 1:
            raise ValueError("Failed to start SDDS page.")
        # Set parameter values
        if sddsdata.SetParameter(x.index, "ParameterA", 1.1) != 1:
            raise ValueError("Failed to set parameter value.")
        # Set array values
        if sddsdata.SetArray(x.index, "ArrayA", [1, 2, 3], [3]) != 1:
            raise ValueError("Failed to set array value.")
        if sddsdata.SetArray(x.index, "ArrayB", [1, 2, 3, 4, 5, 6], [2, 3]) != 1:
            raise ValueError("Failed to set array value.")
        # Set all columns, one row at a time
        if sddsdata.SetRowValues(x.index, 0, ["ColumnA", 7, "ColumnB", 7]) != 1:
            raise ValueError("Failed to set row values.")
        if sddsdata.SetRowValues(x.index, 1, ["ColumnA", 8, "ColumnB", 8]) != 1:
            raise ValueError("Failed to set row values.")
        if sddsdata.SetRowValues(x.index, 2, ["ColumnA", 9, "ColumnB", 9]) != 1:
            raise ValueError("Failed to set row values.")
        # Write page
        if sddsdata.WritePage(x.index) != 1:
            raise ValueError("Failed to write SDDS page.")
        # Close SDDS output file
        if sddsdata.Terminate(x.index) != 1:
            raise ValueError("Failed to terminate SDDS output.")

    except:
        sddsdata.PrintErrors(x.SDDS_VERBOSE_PrintErrors)
        raise

@dataclass
class Description:
    text: Optional[str] = None
    contents: Optional[str] = None
@dataclass
class Parameter:
    name: str
    type: str
    symbol: Optional[Any] = None
    units: Optional[Any] = None
    description: Optional[Any] = None
    format_string: Optional[Any] = None
    fixed_value: Optional[str] = None
@dataclass
class Array:
    name: str
    type: str
    symbol: Optional[Any] = None
    units: Optional[Any] = None
    description: Optional[Any] = None
    format_string: Optional[Any] = None
    group_name: Optional[Any] = None
    field_length: Optional[Any] = None
    dimensions: Optional[Any] = None
@dataclass
class Column:
    name: str
    type: str
    symbol: Optional[Any] = None
    units: Optional[Any] = None
    description: Optional[Any] = None
    format_string: Optional[Any] = None
    field_length: Optional[Any] = None
class SddsFile:
    def __init__(self):
        self.pages = 0
        self.binary = False
        self.definitions = {}
        self.values = {}
        self.array_dims = {}

def read(input_file) -> SddsFile:
    """
    Mostly backward compatible with the PyLHC sdds module read() function.
    Unlike the PyLHC version, this function reads all the SDDS pages and works with column data.
    The data is returned in a structured dictionary.

    Args:
        input_file (str): The input SDDS file to be read.

    Returns:
        dict: A dictionary with parameters, arrays, and columns data.
              Example structure:
              {
                  pages =        {integer},
                  binary =       {boolean},
                  "definitions": {name: {"type:" string, "units": string, "description": string},
                  "values":      {parameter_name: {[Page1Value, Page2Value, ...]},
                                 {array_name:     {[Page1List, Page2List, ...]},
                                 {column_name:    {[Page1List, Page2List, ...]},
                  "values":      {array_name:     {[page1DimList, Page2DimList, ...]}
              }
    """
    # Initialize an SDDS object
    sdds_obj = SDDS()

    # Load the file
    try:
        sdds_obj.load(input_file)
    except Exception as e:
        raise ValueError(f"Failed to load the SDDS file: {e}")

    # Initialize the output dictionary
    sdds_data = SddsFile()

    # Store the data mode (ascii or binary)
    if sdds_obj.mode == SDDS.SDDS_BINARY:
        sdds_data.binary = True
    else:
        sdds_data.binary = False

    # Store description data
    text = sdds_obj.description[0] if sdds_obj.description[0] != "" else None
    contents = sdds_obj.description[1] if sdds_obj.description[1] != "" else None
    description_instance = Description(text=text, contents=contents)
    sdds_data.description = description_instance

    # Store number of pages
    sdds_data.pages = sdds_obj.loaded_pages

    # Store parameter data
    if sdds_obj.parameterName:
        for i, definition in enumerate(sdds_obj.parameterDefinition):
            name = sdds_obj.parameterName[i]
            symbol = definition[0] if definition[0] != '' else None
            units = definition[1] if definition[1] != '' else None
            description = definition[2] if definition[2] != '' else None
            formatString = definition[3] if definition[3] != '' else None
            datatype = sdds_data_type_to_short_string(definition[4])
            fixedValue = definition[5] if definition[5] != '' else None
            parameter_instance = Parameter(name=name, type=datatype, symbol=symbol, units=units, description=description, format_string=formatString, fixed_value=fixedValue)
            sdds_data.definitions.update({name: parameter_instance})
            sdds_data.values.update({name: sdds_obj.parameterData[i]})

    # Store array data
    if sdds_obj.arrayName:
        for i, definition in enumerate(sdds_obj.arrayDefinition):
            name = sdds_obj.arrayName[i]
            symbol = definition[0] if definition[0] != '' else None
            units = definition[1] if definition[1] != '' else None
            description = definition[2] if definition[2] != '' else None
            formatString = definition[3] if definition[3] != '' else None
            group_name = definition[4] if definition[4] != '' else None
            datatype = sdds_data_type_to_short_string(definition[5])
            fieldLength = definition[6] if definition[6] != 0 else None
            dimensions = definition[7] if definition[7] != '' else None
            array_instance = Array(name=name, type=datatype, symbol=symbol, units=units, description=description, format_string=formatString, group_name=group_name, field_length=fieldLength, dimensions=dimensions)
            sdds_data.definitions.update({name: array_instance})
            sdds_data.values.update({name: sdds_obj.arrayData[i]})
            sdds_data.array_dims.update({name: sdds_obj.arrayDimensions[i]})

    # Store column data
    if sdds_obj.columnName:
        for i, definition in enumerate(sdds_obj.columnDefinition):
            name = sdds_obj.columnName[i]
            symbol = definition[0] if definition[0] != '' else None
            units = definition[1] if definition[1] != '' else None
            description = definition[2] if definition[2] != '' else None
            formatString = definition[3] if definition[3] != '' else None
            datatype = sdds_data_type_to_short_string(definition[4])
            fieldLength = definition[5] if definition[5] != 0 else None
            column_instance = Column(name=name, type=datatype, symbol=symbol, units=units, description=description, format_string=formatString, field_length=fieldLength)
            sdds_data.definitions.update({name: column_instance})
            sdds_data.values.update({name: sdds_obj.columnData[i]})

    # Return the dictionary
    return sdds_data

def write(sdds_file: SddsFile, output_file):
    """
    Mostly backward compatible with the PyLHC sdds module write() function.
    Unlike the PyLHC version, this function writes all the SDDS pages and works with column data.
    The data is expected to be in a structured dictionary like that returned from the read() function.

    Args:
        output_file (str): The output SDDS file to be written to.
    """
    # Initialize an SDDS object
    sdds_obj = SDDS()

    # Set the data mode (ascii or binary)
    if hasattr(sdds_file, 'binary'):
        if sdds_file.binary:
            sdds_obj.mode = SDDS.SDDS_BINARY
        else:
            sdds_obj.mode = SDDS.SDDS_ASCII

    # Set description data
    if hasattr(sdds_file, 'description'):
        text = sdds_file.description.text if sdds_file.description.text != None else ''
        contents = sdds_file.description.contents if sdds_file.description.contents != None else ''
        sdds_obj.setDescription(text, contents)
    
    for name in sdds_file.definitions:
        if isinstance(sdds_file.definitions[name], Parameter):
            # Set parameter data
            parameter_instance = sdds_file.definitions[name]
            symbol = parameter_instance.symbol if parameter_instance.symbol != None else ""
            units = parameter_instance.units if parameter_instance.units != None else ""
            description = parameter_instance.description if parameter_instance.description != None else ""
            formatString = parameter_instance.format_string if parameter_instance.format_string != None else ""
            datatype = sdds_short_string_to_data_type(parameter_instance.type)
            fixedValue = parameter_instance.fixed_value if parameter_instance.fixed_value != None else ""
            sdds_obj.defineParameter(name, symbol=symbol, units=units, description=description,
                                     formatString=formatString, type=datatype, fixedValue=fixedValue)
            sdds_obj.setParameterValueList(name, sdds_file.values[name])
        elif isinstance(sdds_file.definitions[name], Array):
            # Set array data
            array_instance = sdds_file.definitions[name]
            symbol = array_instance.symbol if array_instance.symbol != None else ""
            units = array_instance.units if array_instance.units != None else ""
            description = array_instance.description if array_instance.description != None else ""
            formatString = array_instance.format_string if array_instance.format_string != None else ""
            group_name = array_instance.group_name if array_instance.group_name != None else ""
            datatype = sdds_short_string_to_data_type(array_instance.type)
            fieldLength = array_instance.field_length if array_instance.field_length != None else 0
            dimensions = array_instance.dimensions if array_instance.dimensions != None else 1
            sdds_obj.defineArray(name, symbol=symbol, units=units, description=description,
                                 formatString=formatString, group_name=group_name, type=datatype,
                                 fieldLength=fieldLength, dimensions=dimensions)
            sdds_obj.setArrayValueLists(name, sdds_file.values[name], sdds_file.array_dims[name])
        elif isinstance(sdds_file.definitions[name], Column):
            # Set column data
            column_instance = sdds_file.definitions[name]
            symbol = column_instance.symbol if column_instance.symbol != None else ""
            units = column_instance.units if column_instance.units != None else ""
            description = column_instance.description if column_instance.description != None else ""
            formatString = column_instance.format_string if column_instance.format_string != None else ""
            datatype = sdds_short_string_to_data_type(column_instance.type)
            fieldLength = column_instance.field_length if column_instance.field_length != None else 0
            sdds_obj.defineColumn(name, symbol=symbol, units=units, description=description,
                                  formatString=formatString, type=datatype, fieldLength=fieldLength)
            sdds_obj.setColumnValueLists(name, sdds_file.values[name])

    # Set the data to the output file
    sdds_obj.save(output_file)
