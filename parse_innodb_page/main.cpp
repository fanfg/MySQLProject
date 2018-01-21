#include <iostream>
#include <fstream>
#include <assert.h>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include "innodb_page.h"
using std::cout;
using std::endl;


void get_table_record_count(const std::vector<Innodbpage>  *iv) {

	assert((iv->size()) >0 ? true : false);
	std::map<uint64_t, uint64_t> indexid_n_record;
	for (std::vector<Innodbpage>::const_iterator ivc = iv->cbegin(); ivc != iv->cend(); ivc++) {
		/*B tree and  leaf node*/
		if (ivc->filhead.file_page_type == 0x45bf && ivc->pagehead.page_level == 0) {
			indexid_n_record[ivc->pagehead.page_index_id] += ivc->pagehead.page_n_recs;
		}
	}

	//for (std::map<uint64_t, uint64_t>::const_iterator mc = indexid_n_record.cbegin(); mc != indexid_n_record.cend(); mc++) {
	//	cout << "index_id : " << mc->first << " record_count: " << mc->second << endl;
	//}


	std::for_each(indexid_n_record.cbegin(), indexid_n_record.cend(), [](std::map<uint64_t, uint64_t>::value_type   mc) {cout << "index_id : " << mc.first << " record_count: " << mc.second << endl; });


}

int main(int argc, char **argv) {
	std::string file_name = argv[1];
	//"E:\MySQL\data\test\test.ibd"
	std::ifstream fil(file_name , std::ifstream::binary);
	if (!fil.is_open())
		assert("file is not open");
	fil.seekg(0, std::ios::end);
	uint64_t file_size = fil.tellg();
	size_t n_page = file_size / INNODB_PAGE_SZIE;
	std::cout << "page count :" << n_page << endl;
	std::vector<Innodbpage> innodbpages;
	for (size_t i = 0; i < n_page; i++) {
		fil.seekg(i*INNODB_PAGE_SZIE);
		innodbpages.push_back(Innodbpage(fil));
	}
	for (std::vector<Innodbpage>::const_iterator vi = innodbpages.cbegin(); vi != innodbpages.cend(); vi++) {
		cout << "space_id: " << vi->filhead.file_page_space_id << " page : " << vi->filhead.file_page_offset << " page type : " << m_page_types[vi->filhead.file_page_type] << endl;
	}

	get_table_record_count(&innodbpages);

	for (std::vector<Innodbpage>::const_iterator ivc = innodbpages.cbegin(); ivc != innodbpages.cend(); ivc++) {
		/*B tree and  leaf node*/
		if (ivc->filhead.file_page_type == 0x45bf && ivc->pagehead.page_level == 0) {
			cout << "record_type: " << int(ivc->infisupre.infirechead.record_type) << " order: " << ivc->infisupre.infirechead.record_order << " infivalue: " << ivc->infisupre.infivalue << endl;
			cout << "record_type: " << int(ivc->infisupre.suprerechead.record_type) << " order: " << ivc->infisupre.suprerechead.record_order << " suprevalue: " << std::string(ivc->infisupre.suprevalue ,8)<< endl;
			for (std::vector<Rec>::const_iterator vr = ivc->recs.cbegin(); vr != ivc->recs.cend(); vr++) {
				cout << "record_type " << int(vr->rechead.record_type) << " order:" << vr->rechead.record_order << " Id: " << vr->id << " offset:"<<vr->rechead.record_next_offset<<endl;
			}
		}
	}

	std::string file_frm = file_name.replace(file_name.size()-3,3, "frm");
	std::ifstream fil_frm(file_frm, std::ifstream::binary);
	get_table_colname_coltype(fil_frm);
	//int col_type= enum_field_types::

}
