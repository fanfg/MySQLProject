#ifndef INNODB_PAGE_H
#define INNODB_PAGE_H
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include "../parse_binlog/binlog.h"

/*innodb page size default 16K */
const int INNODB_PAGE_SZIE = 16 * 1024;

/*
innodb_page_types={
'0000':'Freshly Allocated Page',
'0002':'Undo Log Page',
'0003':'File Segment inode',
'0004':'Insert Buffer Free List',
'0005':'Insert Buffer Bitmap',
'0006':'System Page',
'0007':'Transaction system Page',
'0008':'File Space Header',
'0009':'extra description page',
'000a':'Uncompressed BLOB Page',
'000b':'1st compressed BLOB Page',
'000c':'Subsequent compressed BLOB Page',
'45bf':'B-tree Node'
}
*/
std::map<uint16_t, std::string> m_page_types = { { 0x0000,"Freshly Allocated Page" },{ 0x0002,"Undo Log Page" } ,{ 0x0003,"File Segment inode" },
{ 0x0004,"Insert Buffer Free List" },{ 0x0005,"Insert Buffer Bitmap" },{ 0x0006,"System Page" } ,
{ 0x0007,"Transaction system Page" },{ 0x0008,"File Space Header" },{ 0x0009,"extra description page" },
{ 0x000a,"Uncompressed BLOB Page" },{ 0x000b,"1st compressed BLOB Page" },{ 0x000c,"Subsequent compressed BLOB Page" },
{ 0x45bf,"B-tree Node" } };

/*
innodb_page_direction={
'0000': 'Unknown(0x0000)',
'0001': 'Page Left',
'0002': 'Page Right',
'0003': 'Page Same Rec',
'0004': 'Page Same Page',
'0005': 'Page No Direction',
'ffff':  'Unkown2(0xffff)'
}
*/




enum type_size {
	short_type = 2,
	int_type = 4,
	ll_type = 8
};

template <typename T>
void byte_swap(std::ifstream &is, T &num) {
	is.read((char *)&num, sizeof(T));
	switch (sizeof(T))
	{
	case short_type:
		num = _byteswap_ushort(num);
		break;
	case int_type:
		num = _byteswap_ulong(num);
		break;
	case ll_type:
		num = _byteswap_uint64(num);
		break;
	default:
		break;
	}
}

void  get_table_colname_coltype(std::ifstream &is, std::vector<std::string>* v_column_name, std::vector< uint8_t>* v_column_type ) {
	
	v_column_name->clear();
	v_column_type->clear();
	is.seekg(0x2102);
	/* column counts */
	uint16_t n_columns;
	is.read((char *) &n_columns,2); 
	n_columns = n_columns & 0x7fff;
	is.seekg(0x2150);
	
	/*get the vector of the field name */
	for (int i = 0; i < n_columns; i++) {
		uint8_t order;
		is.read((char *)&order, 1);
		uint8_t com_len;
		is.read((char *)&com_len, 1);
		uint64_t colname_length = UnPackedLength(is);
		char *colname = new char(colname_length);
		is.read(colname, colname_length );
		std::cout << " col "<<i<<" : " << colname << endl;
		v_column_name->push_back(colname);
	
	}

	/*get the vector of the field type */
	for (int i = 0; i < n_columns; i++) {
		uint8_t section_filed[17];
		is.read((char *)section_filed, 17);
		//std::cout << " col " << i << " : " << int(section_filed[13]) << endl;
		v_column_type->push_back(  int(section_filed[13]));

	}






}


/*total 38 bytes*/
class Fileheader {
public:
	/*after mysql4.0.14   page checksum  4 bytes*/
	uint32_t file_page_space_checksum;
	/* ordinal page number from start of space   2 bytes*/
	uint32_t  file_page_offset;
	/* offset of previous page in key order*/
	uint32_t  file_page_prev;
	/* offset of next page in key order */
	uint32_t  file_page_next;
	/* log serial number of page's latest log record */
	uint64_t  file_page_lsn;
	/* current defined types are: FIL_PAGE_INDEX, FIL_PAGE_UNDO_LOG, FIL_PAGE_INODE, FIL_PAGE_IBUF_FREE_LIST */
	uint16_t file_page_type;
	/* 1 only appear in system tablespace and user tablespace it always 0
	2 the file has been flushed to disk at least up to this lsn" (log serial number), valid only on the first page of the file*/
	uint64_t file_page_file_flush_lsn;
	/* this page belongs to which tablespace id  (after mysql 4.1) */
	uint32_t file_page_space_id;

	Fileheader() = default;
	Fileheader(std::ifstream &is) {
		byte_swap(is, file_page_space_checksum);
		byte_swap(is, file_page_offset);
		byte_swap(is, file_page_prev);
		byte_swap(is, file_page_next);
		byte_swap(is, file_page_lsn);
		byte_swap(is, file_page_type);
		byte_swap(is, file_page_file_flush_lsn);
		byte_swap(is, file_page_space_id);
	}
	std::string get_page_type() {
		return m_page_types[file_page_type];
	}

};

/* total 56 bytes*/
class Pageheader {
public:
	/* number of directory slots in the Page Directory part; initial value = 2(infimum and supremum)*/
	uint16_t  page_n_dir_slots;
	/* record pointer to first record in heap  where freespace start */
	uint16_t  page_heap_top;
	/* number of heap records; initial value = 2 (redundant)  0x8002(compact)*/
	uint16_t  page_n_heap;
	/* record pointer to first free record*/
	uint16_t  page_free;
	/* number of bytes in deleted records*/
	uint16_t  page_garbage;
	/* record pointer to the last inserted record */
	uint16_t  page_last_insert;
	/*  either PAGE_LEFT, PAGE_RIGHT, or PAGE_NO_DIRECTION  */
	uint16_t  page_direction;
	/* number of consecutive inserts in the same direction, for example, "last 5 were all to the left" */
	uint16_t  page_n_direction;
	/* number of user records */
	uint16_t  page_n_recs;
	/* the highest ID of a transaction which might have changed a record on the page (only set for secondary indexes) */
	uint64_t  page_max_trx_id;
	/* level within the index (0 for a leaf page)*/
	uint16_t  page_level;
	/* identifier of the index the page belongs to*/
	uint64_t  page_index_id;
	/* file segment header for the leaf pages in a B-tree" (this is irrelevant here) */
	char page_btr_seg_leaf[10];
	/* file segment header for the non-leaf pages in a B-tree" (this is irrelevant here) */
	char page_btr_seg_top[10];

	Pageheader() = default;
	Pageheader(std::ifstream &is) {
		byte_swap(is, page_n_dir_slots);
		byte_swap(is, page_heap_top);
		byte_swap(is, page_n_heap);
		page_n_heap = page_n_heap & INT16_MAX;
		byte_swap(is, page_free);
		byte_swap(is, page_garbage);
		byte_swap(is, page_last_insert);
		byte_swap(is, page_direction);
		byte_swap(is, page_n_direction);
		byte_swap(is, page_n_recs);
		byte_swap(is, page_max_trx_id);
		byte_swap(is, page_level);
		byte_swap(is, page_index_id);
		is.read(page_btr_seg_leaf, 10);
		is.read(page_btr_seg_top, 10);

	}

};


class  Recheadfixed {
public:
	/*del bit mask  4 bit*/
	uint8_t record_info;
	/* this slot own  n record  4 bit*/
	uint8_t n_owned;
	/* key logical order 13 bit*/
	uint16_t record_order;
	/* 3 bit
	0 conventional record
	1 node pointer record
	2 infimum record
	3 suprenum record
	1XX reserved*/
	uint8_t  record_type;
	/*next record offset*/
	uint16_t record_next_offset;
	Recheadfixed() = default;
	Recheadfixed(std::ifstream &is) {
		uint8_t first_byte;
		is.read((char *)&first_byte, 1);
		record_info = first_byte / 16;
		n_owned = first_byte % 16;

		uint16_t twobytes;
		byte_swap(is, twobytes);
		record_order = twobytes / 8;
		record_type = twobytes % 8;

		byte_swap(is, record_next_offset);

	}

};


class Infisupre {
public:
	/* fix 5 bytes*/
	Recheadfixed infirechead;
	/*Infimum */
	char infivalue[8];
	Recheadfixed suprerechead;
	/* Supremum */
	char suprevalue[8];
	Infisupre() = default;
	Infisupre(std::ifstream &is) {
		infirechead = Recheadfixed(is);
		is.read(infivalue, 8);
		suprerechead = Recheadfixed(is);
		is.read(suprevalue, 8);

	}


};

struct
	filed_section {
	int column_order;
	std::string column_name;
	uint8_t column_type;
	uint16_t column_metadata_len;
	uint16_t column_data_len;
};

class Rec {
public:
	Recheadfixed rechead;
	uint64_t trx_id;
	uint8_t rec[21];
	Rec() = default;
	Rec(std::ifstream &is) {
		uint32_t pos = is.tellg();
		rechead = Recheadfixed(is);
		is.read((char *)rec, 21);
		is.seekg(pos + rechead.record_next_offset);
	}
};


/*
#An InnoDB page has seven parts:
#1: Fil Header 38 bytes
#2: Page Header 56 bytes
#3: Infimum + Supremum Records   26 bytes  when  innodb_default_row_format=Dynamic ( variant of Compact)   (Redundant is not 26 bytes)
#4: User Records
#5: Free Space
#6: Page Directory
#7: File Trailer
*/
class Innodbpage {
public:
	Fileheader filhead;
	Pageheader pagehead;
	Infisupre  infisupre;
	std::vector<Rec> recs;
	Innodbpage() = default;
	Innodbpage(std::ifstream &is) :filhead(is), pagehead(is), infisupre(is) {
		int pos = filhead.file_page_offset*INNODB_PAGE_SZIE+99;
		if (filhead.file_page_type == 0x45bf) {
			is.seekg(pos + infisupre.infirechead.record_next_offset-5);
			for (int i = 0; i < pagehead.page_n_recs; i++)
				recs.push_back(Rec(is));
			


		}
	}
};

#endif // INNODB_PAGE_H
