#include "ppiseq.h"

using namespace std;


vector<tstring> tsplit(const tstring &str, const tstring &delim)
{
	vector<tstring> res;
	size_t current = 0, found;
	while ((found = str.find_first_of(delim, current)) != string::npos) {
		res.push_back(tstring(str, current, found - current));
		current = found + 1;
	}
	res.push_back(tstring(str, current, str.size() - current));
	return res;
}

vector<string> split(const string &str, const string &delim)
{
	vector<string> res;
	size_t current = 0, found;
	while ((found = str.find_first_of(delim, current)) != string::npos) {
		res.push_back(string(str, current, found - current));
		current = found + 1;
	}
	res.push_back(string(str, current, str.size() - current));
	return res;
}


int _tmain(int argc, TCHAR* argv[])
{
	ios_base::sync_with_stdio(false);
	locale default_loc("");
	locale::global(default_loc);
	locale ctype_default(locale::classic(), default_loc, locale::ctype);
	tcout.imbue(ctype_default);
	tcin.imbue(ctype_default);
	tcin.unsetf(std::ios::skipws);

	if (argc == 1) {
		tcout << _T("no input file specified!\npress any key to exit.") << endl;
		while(!_kbhit());
	}
	else if (argc > 1) {
		for (int i = 1; i < argc; i++) {
			tcout << argv[i] << endl;
		}

		tstring ref;
		tcout << _T("Input a reference sequence.") << endl;
		tcin >> ref;

		Sequence refseq(ref);
		if (!refseq.is_valid()) {
			tcout << _T("Invalid sequence.\n") << refseq.get_sequence() << endl;
			tcout << _T("press any key to exit.") << endl;
			while(!_kbhit());
			return -1;
		}

		tstring rlimit_str;
		int rlimit = INT_MAX;
		tcout << _T("Enter a lower limit of rank: ");
		
		tcin.ignore();
		tcin >> rlimit_str;
		if (!rlimit_str.empty() && rlimit_str.find_first_not_of(_T("0123456789.")) == string::npos) {
			rlimit = _ttoi(rlimit_str.c_str());
		}
		tcin.setf(std::ios::skipws);
		
		tcout << refseq.get_sequence() << endl;

		for (int i = 1; i < argc; i++) {
			vector<Sequence> seq_list;
			vector<Sequence> results;
			tifstream ifs(argv[i]);
			if (ifs.fail()) {
				tcerr << _T("file open error: ") << argv[i] << endl;
				continue;
			}

			tcout << _T("loading data...") << endl;;
			tstring line, token;
			while (getline(ifs, line)) {
				vector<tstring> tokens = tsplit(line, _T(" \t,"));
				int tknum = tokens.size();
				bool val_set = false;
				Sequence seq;
				for (int j = 0; j < tknum; j++) {
					if (!seq.is_valid()) {
						seq.set_sequence(tokens[j]);
						if (!seq.is_valid()) continue;
					}
					else if (!tokens[j].empty() && 
						tokens[j].find_first_not_of(_T("0123456789.")) == string::npos) {
						seq.set_dval(_ttof(tokens[j].c_str()));
						val_set = true;
						break;
					}
				}

				if (seq.is_valid() && val_set) seq_list.push_back(seq);
			}

			sort(seq_list.begin(), seq_list.end(), Sequence::high_dval);

			int rank = 0;
			double before_dval = DBL_MAX;
			int listsize = seq_list.size();
			double proc_unit = listsize/10;
			int proc = 0;
			tcout << _T("processing");
			for (unsigned int j = 0; j < listsize; j++) {

				proc++;
				if (proc >= proc_unit) {
					tcout << _T(".");
					proc = 0;
				}

				int offset = INT_MAX, mnum = 0, c_offset = INT_MAX, c_mnum = 0;
				bool rank_down = false;
				double tmp_bf_dval = before_dval; 
				Sequence seq = seq_list[j];
				
				if (before_dval != seq.get_dval()) {
					before_dval = seq.get_dval();
					rank++;
					rank_down = true;
					if (rank > rlimit) break;
				}
				seq_list[j].set_ival(rank);
				seq.set_ival(rank);

				seq.find_best_match_offset(refseq, offset, mnum);
				seq.make_complement();
				seq.find_best_match_offset(refseq, c_offset, c_mnum);
				if (offset != 0 && c_offset != 0) {
					if (rank_down) {
						before_dval = tmp_bf_dval;
						rank--;
					}
					continue;
				}
				else if (offset == 0 && c_offset != 0) {results.push_back(seq_list[j]);}
				else if (offset != 0 && c_offset == 0) {results.push_back(seq);}
				else if (offset == 0 && c_offset == 0) {
					if      (mnum > c_mnum) {results.push_back(seq_list[j]);}
					else if (mnum < c_mnum) {results.push_back(seq);}
					else if (mnum == c_mnum) {
						double halfval = seq_list[j].get_dval()/2.0;
						seq_list[j].set_dval(halfval);
						seq.set_dval(halfval);
						results.push_back(seq_list[j]);
						results.push_back(seq);
					}
				}
			}
			tcout << _T(".") << endl;;

			int reflen = refseq.get_sequence().length();
			vector<double> a(reflen ,0.0), t(reflen, 0.0), c(reflen, 0.0), g(reflen, 0.0);
			for (unsigned int j = 0; j < results.size(); j++) {
				tstring seqstr = results[j].get_sequence();
				if (reflen == seqstr.length()) {
					for (int k = 0; k < reflen; k++) {
						if (Sequence::is_base_match(seqstr[k], 'A')) a[k] += results[j].get_dval();
						if (Sequence::is_base_match(seqstr[k], 'T')) t[k] += results[j].get_dval();
						if (Sequence::is_base_match(seqstr[k], 'G')) g[k] += results[j].get_dval();
						if (Sequence::is_base_match(seqstr[k], 'C')) c[k] += results[j].get_dval();
					}
				}
			}

			//output
			tcout << _T("saving results...") << endl;
			tstring tar_fpath = argv[i];
			int ext_pos = tar_fpath.find_last_of(_T("."));
			if (ext_pos != string::npos) tar_fpath = tar_fpath.substr(0, ext_pos);
			tar_fpath += _T("_result.txt");
			tofstream ofs(tar_fpath);
			ofs.imbue(ctype_default);
			if (ofs.fail()) {
				tcerr << _T("file open error: ") << tar_fpath << endl;
			}
			else
			{
				for (unsigned int j = 0; j < results.size(); j++) {
					ofs << results[j].get_ival() << _T("\t")<< results[j].get_sequence() << _T("\t") << results[j].get_dval() << endl;
				}
				ofs << endl;

				ofs << _T("A:\t");
				for (int j = 0; j < reflen; j++) ofs << a[j] << _T("\t"); 
				ofs << endl;
				ofs << _T("T:\t");
				for (int j = 0; j < reflen; j++) ofs << t[j] << _T("\t"); 
				ofs << endl;
				ofs << _T("G:\t");
				for (int j = 0; j < reflen; j++) ofs << g[j] << _T("\t"); 
				ofs << endl;
				ofs << _T("C:\t");
				for (int j = 0; j < reflen; j++) ofs << c[j] << _T("\t"); 
				ofs << endl;

				ofs.close();
			}

			ifs.close();
		}

		tcout << _T("press any key to exit.") << endl;
		while(!_kbhit());
	}
	
	return 0;
}
