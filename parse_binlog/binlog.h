#ifndef BINLOG_H_MYCPP
#define BINLOG_H_MYCPP
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
using std::cout;
using std::endl;




/**
Enumeration type for the different types of log events.
*/
enum Log_event_type {
	/**
	Every time you update this enum (when you add a type), you have to
	fix Format_description_event::Format_description_event().
	*/
	UNKNOWN_EVENT = 0,
	START_EVENT_V3 = 1,
	QUERY_EVENT = 2,
	STOP_EVENT = 3,
	ROTATE_EVENT = 4,
	INTVAR_EVENT = 5,
	LOAD_EVENT = 6,
	SLAVE_EVENT = 7,
	CREATE_FILE_EVENT = 8,
	APPEND_BLOCK_EVENT = 9,
	EXEC_LOAD_EVENT = 10,
	DELETE_FILE_EVENT = 11,
	/**
	NEW_LOAD_EVENT is like LOAD_EVENT except that it has a longer
	sql_ex, allowing multibyte TERMINATED BY etc; both types share the
	same class (Load_event)
	*/
	NEW_LOAD_EVENT = 12,
	RAND_EVENT = 13,
	USER_VAR_EVENT = 14,
	FORMAT_DESCRIPTION_EVENT = 15,
	XID_EVENT = 16,
	BEGIN_LOAD_QUERY_EVENT = 17,
	EXECUTE_LOAD_QUERY_EVENT = 18,

	TABLE_MAP_EVENT = 19,

	/**
	The PRE_GA event numbers were used for 5.1.0 to 5.1.15 and are
	therefore obsolete.
	*/
	PRE_GA_WRITE_ROWS_EVENT = 20,
	PRE_GA_UPDATE_ROWS_EVENT = 21,
	PRE_GA_DELETE_ROWS_EVENT = 22,

	/**
	The V1 event numbers are used from 5.1.16 until mysql-trunk-xx
	*/
	WRITE_ROWS_EVENT_V1 = 23,
	UPDATE_ROWS_EVENT_V1 = 24,
	DELETE_ROWS_EVENT_V1 = 25,

	/**
	Something out of the ordinary happened on the master
	*/
	INCIDENT_EVENT = 26,

	/**
	Heartbeat event to be send by master at its idle time
	to ensure master's online status to slave
	*/
	HEARTBEAT_LOG_EVENT = 27,

	/**
	In some situations, it is necessary to send over ignorable
	data to the slave: data that a slave can handle in case there
	is code for handling it, but which can be ignored if it is not
	recognized.
	*/
	IGNORABLE_LOG_EVENT = 28,
	ROWS_QUERY_LOG_EVENT = 29,

	/** Version 2 of the Row events */
	WRITE_ROWS_EVENT = 30,
	UPDATE_ROWS_EVENT = 31,
	DELETE_ROWS_EVENT = 32,

	GTID_LOG_EVENT = 33,
	ANONYMOUS_GTID_LOG_EVENT = 34,

	PREVIOUS_GTIDS_LOG_EVENT = 35,

	TRANSACTION_CONTEXT_EVENT = 36,

	VIEW_CHANGE_EVENT = 37,

	/* Prepared XA transaction terminal event similar to Xid */
	XA_PREPARE_LOG_EVENT = 38,
	/**
	Add new events here - right above this comment!
	Existing events (except ENUM_END_EVENT) should never change their numbers
	*/
	ENUM_END_EVENT /* end marker */
};



typedef enum enum_field_types {
	MYSQL_TYPE_DECIMAL, MYSQL_TYPE_TINY,
	MYSQL_TYPE_SHORT, MYSQL_TYPE_LONG,
	MYSQL_TYPE_FLOAT, MYSQL_TYPE_DOUBLE,
	MYSQL_TYPE_NULL, MYSQL_TYPE_TIMESTAMP,
	MYSQL_TYPE_LONGLONG, MYSQL_TYPE_INT24,
	MYSQL_TYPE_DATE, MYSQL_TYPE_TIME,
	MYSQL_TYPE_DATETIME, MYSQL_TYPE_YEAR,
	MYSQL_TYPE_NEWDATE, MYSQL_TYPE_VARCHAR = 15,
	MYSQL_TYPE_BIT,
	MYSQL_TYPE_TIMESTAMP2,
	MYSQL_TYPE_DATETIME2,
	MYSQL_TYPE_TIME2,
	MYSQL_TYPE_JSON = 245,
	MYSQL_TYPE_NEWDECIMAL = 246,
	MYSQL_TYPE_ENUM = 247,
	MYSQL_TYPE_SET = 248,
	MYSQL_TYPE_TINY_BLOB = 249,
	MYSQL_TYPE_MEDIUM_BLOB = 250,
	MYSQL_TYPE_LONG_BLOB = 251,
	MYSQL_TYPE_BLOB = 252,
	MYSQL_TYPE_VAR_STRING = 253,
	MYSQL_TYPE_STRING = 254,
	MYSQL_TYPE_GEOMETRY = 255
} enum_field_types;

//need to format the ident
//unsigned long long getMetadataLength(uint8_t fieldType, std::string dataBuffer)
uint16_t getMetadataLength(uint8_t fieldType) {
	uint16_t  metadataLength = 0;
	uint16_t  length;
	length = 0;

	switch ((fieldType)) {
	case MYSQL_TYPE_NEWDECIMAL:
		metadataLength = 2;
		break;
	case MYSQL_TYPE_DECIMAL:
	case MYSQL_TYPE_FLOAT:
	case MYSQL_TYPE_DOUBLE:
		metadataLength = 1;
		break;
		/*
		The cases for SET and ENUM are include for completeness, however
		both are mapped to type MYSQL_TYPE_STRING and their real types
		are encoded in the field metadata.
		*/
	case MYSQL_TYPE_SET:
	case MYSQL_TYPE_ENUM:
	case MYSQL_TYPE_STRING:
		metadataLength = 2;
		break;
	case MYSQL_TYPE_YEAR:
	case MYSQL_TYPE_TINY:
		length = 1;
		break;
	case MYSQL_TYPE_SHORT:
		length = 2;
		break;
	case MYSQL_TYPE_INT24:
		length = 3;
		break;
	case MYSQL_TYPE_LONG:
		length = 4;
		break;
	case MYSQL_TYPE_LONGLONG:
		length = 8;
		break;
	case MYSQL_TYPE_NULL:
		length = 0;
		break;
	case MYSQL_TYPE_NEWDATE:
		length = 3;
		break;
	case MYSQL_TYPE_DATE:
	case MYSQL_TYPE_TIME:
		length = 3;
		break;
	case MYSQL_TYPE_TIME2:
		/*
		The original methods in the server to calculate the binary size of the
		packed numeric time representation is defined in my_time.c, the signature
		being  unsigned int my_time_binary_length(uint)
		The length below needs to be updated if the above method is updated in
		the server
		*/
		metadataLength = 1;
		break;
	case MYSQL_TYPE_TIMESTAMP:
		length = 4;
		break;
	case MYSQL_TYPE_TIMESTAMP2:
		/*
		The original methods in the server to calculate the binary size of the
		packed numeric time representation is defined in time.c, the signature
		being  unsigned int my_timestamp_binary_length(uint)
		The length below needs to be updated if the above method is updated in
		the server
		*/
		metadataLength = 1;
		break;
	case MYSQL_TYPE_DATETIME:
		length = 8;
		break;
	case MYSQL_TYPE_DATETIME2:
		/*
		The original methods in the server to calculate the binary size of the
		packed numeric time representation is defined in time.c, the signature
		being  unsigned int my_datetime_binary_length(uint)
		The length below needs to be updated if the above method is updated in
		the server
		*/
		metadataLength = 1;
		break;
	case MYSQL_TYPE_BIT: {
		/*
		Decode the size of the bit field from the master.
		from_len is the length in bytes from the master
		from_bit_len is the number of extra bits stored in the master record
		If from_bit_len is not 0, add 1 to the length to account for accurate
		number of bytes needed.
		*/
		metadataLength = 2;
		break;
	}
	case MYSQL_TYPE_VARCHAR: {
		metadataLength = 2;
		break;
	}
	case MYSQL_TYPE_TINY_BLOB:
	case MYSQL_TYPE_MEDIUM_BLOB:
	case MYSQL_TYPE_LONG_BLOB:
	case MYSQL_TYPE_BLOB:
	case MYSQL_TYPE_GEOMETRY:
	case MYSQL_TYPE_JSON: {
		/*
		Compute the length of the data. We cannot use get_length() here
		since it is dependent on the specific table (and also checks the
		packlength using the internal 'table' pointer) and replication
		is using a fixed format for storing data in the binlog.
		*/
		metadataLength = 1;
		break;
	}
	}
	return metadataLength;



}

uint32_t maxDisplayLengthForField(enum_field_types columnType, uint32_t metadata) {
	//BAPI_ASSERT(metadata >> 16 == 0);

	switch (columnType) {
	case MYSQL_TYPE_NEWDECIMAL:
		return metadata >> 8;

	case MYSQL_TYPE_FLOAT:
		return 12;

	case MYSQL_TYPE_DOUBLE:
		return 22;

	case MYSQL_TYPE_SET:
	case MYSQL_TYPE_ENUM:
		return metadata & 0x00ff;

	case MYSQL_TYPE_STRING: {
		uint8_t type = metadata >> 8;
		if (type == MYSQL_TYPE_SET || type == MYSQL_TYPE_ENUM)
			return metadata & 0xff;
		else
			return (((metadata >> 4) & 0x300) ^ 0x300) + (metadata & 0x00ff);
	}

	case MYSQL_TYPE_YEAR:
	case MYSQL_TYPE_TINY:
		return 4;

	case MYSQL_TYPE_SHORT:
		return 6;

	case MYSQL_TYPE_INT24:
		return 9;

	case MYSQL_TYPE_LONG:
		return 11;

	case MYSQL_TYPE_LONGLONG:
		return 20;

	case MYSQL_TYPE_NULL:
		return 0;

	case MYSQL_TYPE_NEWDATE:
		return 3;

	case MYSQL_TYPE_DATE:
	case MYSQL_TYPE_TIME:
	case MYSQL_TYPE_TIME2:
		return 3;

	case MYSQL_TYPE_TIMESTAMP:
	case MYSQL_TYPE_TIMESTAMP2:
		return 4;

	case MYSQL_TYPE_DATETIME:
	case MYSQL_TYPE_DATETIME2:
		return 8;

	case MYSQL_TYPE_BIT:
		/*
		Decode the size of the bit field from the master.
		*/
		//BAPI_ASSERT((metadata & 0xff) <= 7);
		return 8 * (metadata >> 8U) + (metadata & 0x00ff);

	case MYSQL_TYPE_VAR_STRING:
	case MYSQL_TYPE_VARCHAR:
		return metadata;

		/*
		The actual length for these types does not really matter since
		they are used to calc_pack_length, which ignores the given
		length for these types.
		Since we want this to be accurate for other uses, we return the
		maximum size in bytes of these BLOBs.
		*/

	case MYSQL_TYPE_TINY_BLOB:
		return uint32_t(1 * 8);

	case MYSQL_TYPE_MEDIUM_BLOB:
		return uint32_t(3 * 8);

	case MYSQL_TYPE_BLOB:
		/*
		For the blob type, Field::real_type() lies and say that all
		blobs are of type MYSQL_TYPE_BLOB. In that case, we have to look
		at the length instead to decide what the max display size is.
		*/
		return uint32_t(metadata * 8);

	case MYSQL_TYPE_LONG_BLOB:
	case MYSQL_TYPE_GEOMETRY:
	case MYSQL_TYPE_JSON:
		return uint32_t(4 * 8);

	default:
		return UINT_MAX;
	}
}

uint64_t UnPackedLength(std::ifstream &infile) {
	uint64_t column;
	uint8_t firstbyte;
	infile.read((char *)&firstbyte, 1);
	if (firstbyte >= 0 && firstbyte<251)
		column = firstbyte;
	else if (firstbyte == 0xFC)
		infile.read((char *)&column, 2);
	else if (firstbyte == 0xFD)
		infile.read((char *)&column, 3);
	else
		infile.read((char *)&column, 8);
	return column;
}


//binlogevent=BinlogEventHeader (fixed 19 bytes) +BinlogEventData

/*	+=====================================+
| event  | fixed part (post-header)   |
| data   +----------------------------+
|        | variable part (payload)    |
+=====================================+*/

class BinlogEventHeader {
public:
	int  timestamp;
	uint8_t type_code;
	int  server_id;
	int  event_length;
	int  next_position;
	short flags;

	BinlogEventHeader() = default;
	BinlogEventHeader(std::ifstream &infile) {

		infile.read((char *)&timestamp, 4);
		infile.read((char *)&type_code, 1);
		infile.read((char *)&server_id, 4);
		infile.read((char *)&event_length, 4);
		infile.read((char *)&next_position, 4);
		infile.read((char *)&flags, 2);
	}

	inline int GetBinlogEventTyepCode() const {
		return int(type_code);
	}

	void PrintBinlogEventHeader() {
		cout << "BinlogEventHeader\n{\n" << endl;
		cout << "    timestamp: " << this->timestamp << endl;
		cout << "    type_code: " << int(this->type_code) << endl;
		cout << "    server_id: " << this->server_id << endl;
		cout << "    event_length: " << this->event_length << endl;
		cout << "    next_position: " << this->next_position << endl;
		cout << "    flags[]: " << this->flags << "\n } \n";
	}
};


class FormatDescriptionEvent {
public:
	BinlogEventHeader mhead;
	short binlog_version;
	std::string server_version;
	//char server_version[50];
	int create_timestamp;
	uint8_t header_length;
	//fixed part (post-header) length for all event types
	std::vector<uint8_t> post_header_len;

	FormatDescriptionEvent(const BinlogEventHeader &head, std::ifstream &infile) :mhead(head) {
		infile.read((char *)&binlog_version, 2);
		char serverversion[50];
		infile.read((char *)&serverversion, 50);
		server_version.assign(serverversion, serverversion + 50);
		infile.read((char *)&create_timestamp, 4);
		infile.read((char *)&header_length, 1);
		for (int i = 0; i<mhead.event_length - 76; i++) {
			uint8_t tmp;
			infile.read((char *)&tmp, 1);
			post_header_len.push_back(tmp);
		}
	}


	void GetAllEventTypesPostHeaderLength() {
		for (int i = 0; i <post_header_len.size(); i++) {
			printf(" type %d: %d\n", i + 1, post_header_len[i]);
		}
	}

	void PrintFormatDescriptionEventData() {
		mhead.PrintBinlogEventHeader();
		cout << "FormatDescriptionEventData\n{\n" << endl;
		cout << "binlog_version: " << binlog_version << endl;
		cout << "server_version: " << server_version << endl;
		cout << "create_timestamp: " << create_timestamp << endl;
		cout << "header_length: " << int(header_length) << endl;
		GetAllEventTypesPostHeaderLength();
		cout << "\n}" << endl;

	}

};

class TableMapBinlogEvent {
public:
	BinlogEventHeader mhead;
	long tableid;
	short unused;
	uint8_t dbname_length;
	std::string dbname;
	uint8_t tb_length;
	std::string tablename;
	unsigned long long column_length;
	std::vector<uint8_t> columntypearr;
	unsigned long long metadatalength;
	std::vector<uint16_t> metadatablock;
	std::vector<std::uint8_t> nullbit;


	TableMapBinlogEvent(const BinlogEventHeader &head, std::ifstream &infile) :mhead(head) {
		infile.read((char *)&tableid, 6);
		infile.read((char *)&unused, 2);
		//dbname length and dbname
		infile.read((char *)&dbname_length, 1);
		char *dbnam=new char(dbname_length + 1);
		infile.read((char *)&dbnam, dbname_length + 1);
		dbname = std::string(dbnam, dbname_length);
		//tablename and table length
		infile.read((char *)&tb_length, 1);
		char *tbnam=new char(tb_length + 1);
		infile.read((char *)&tbnam, tb_length + 1);
		tablename = std::string(tbnam, tb_length);
		//cloumn lenth (Packed Integer)
		column_length = UnPackedLength(infile);

		for (int i = 0; i<column_length; i++) {
			uint8_t tmp;
			infile.read((char *)&tmp, 1);
			columntypearr.push_back(tmp);
		}
		metadatalength = UnPackedLength(infile);
		for (int i = 0; i<column_length; i++) {
			//get getMetadataLength to   percolumnmetadatalength
			uint16_t percolumnmetadatalength;
			if (getMetadataLength(columntypearr[i]) == 1) {
				infile.read((char *)&percolumnmetadatalength, 1);
			}
			else if (getMetadataLength(columntypearr[i]) == 2) {
				infile.read((char *)&percolumnmetadatalength, 2);
			}
			else
				percolumnmetadatalength = 0;
			metadatablock.push_back(percolumnmetadatalength);

		}
		for (int i = 0; i<(column_length + 7) / 8; i++) {
			//get getMetadataLength to   percolumnmetadatalength
			uint8_t nullbitlength;
			infile.read((char *)&nullbitlength, 1);
			nullbit.push_back(nullbitlength);

		}

	}


	void PrintTableMapBinlogEventtData() {
		mhead.PrintBinlogEventHeader();
		cout << "TableMapBinlogEventData\n{\n" << endl;
		cout << "tableid: " << tableid << endl;
		cout << "unused: " << unused << endl;
		cout << "dbname_length: " << int(dbname_length) << endl;
		cout << "dbname: " << dbname << endl;
		cout << "tb_length: " << int(tb_length) << endl;
		cout << "tablename: " << tablename << endl;
		cout << "column_length: " << column_length << endl;
		cout << "column_array: " << endl;
		for (int i = 0; i <columntypearr.size(); i++) {
			printf(" @%d: %d\n", i + 1, columntypearr[i]);
		}
		cout << "metadatalength: " << metadatalength << endl;
		cout << "metadata_array: \n";
		for (int i = 0; i <columntypearr.size(); i++) {
			printf(" @%d: %d,metadata= %d\n", i + 1, columntypearr[i], metadatablock[i]);
		}
		cout << "nullbit: " << nullbit.size() << " bytes" << endl;
		for (int i = 0; i<(column_length + 7) / 8; i++) {
			cout << int(nullbit[i]) << endl;;
		}
		cout << "\n}" << endl;

	}
	/*	protected:
	friend unsigned long long getMetadataLength(uint8_t fieldType);
	friend unsigned long long  UnPackedLength(std::ifstream &ifstream);*/
	/*
	char column_number;
	infile.read((char *)&column_number,1);
	format_description_event_header.event_length-=1;
	cout<<"column_number:"<<int(column_number)<<endl;
	for (int i=1;i<=int(column_number);i++){
	char column_type;
	infile.read((char*)&column_type,1);
	cout<<"@"<<i<<": "<<int(column_type)<<endl;
	format_description_event_header.event_length-=1;
	}

	char metadata_length;
	infile.read((char *)&metadata_length,1);
	format_description_event_header.event_length-=1;
	cout<<"metadata_length:"<<int(metadata_length)<<endl;

	if(current_binlog_position==605){
	for (int i=1;i<=int(metadata_length);i++){
	unsigned char metadata;
	infile.read((char *)&metadata,1);
	format_description_event_header.event_length-=1;
	cout<<"@"<<i<<": "<<int(metadata)<<endl;
	}
	}*/



};


class InsertRowsBinlogEvent :public BinlogEventHeader {
};

class DeleteRowsBinlogEvent :public BinlogEventHeader {
};

class UpdateRowsBinlogEvent :public BinlogEventHeader {
};





#endif

