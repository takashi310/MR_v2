#include "ppiseq.h"

using namespace std;


bool Sequence::is_base_match(TCHAR b1, TCHAR b2)
{
	int i, j;

	if (b1 == b2 || b1 == 'N' || b2 == 'N') return true;

	TCHAR b[2][3] = { {b1, 'X', 'X'}, {b2, 'X', 'X'} };
	int n[2] = {1, 1};

	for (i = 0; i < 2; i++) {
		switch (b[i][0]) {
			case 'W':
				b[i][0] = 'A'; b[i][1] = 'T';
				n[i] = 2;
				break;
			case 'R':
				b[i][0] = 'A'; b[i][1] = 'G';
				n[i] = 2;
				break;
			case 'Y':
				b[i][0] = 'C'; b[i][1] = 'T';
				n[i] = 2;
				break;
			case 'S':
				b[i][0] = 'G'; b[i][1] = 'C';
				n[i] = 2;
				break;
			case 'K':
				b[i][0] = 'G'; b[i][1] = 'T';
				n[i] = 2;
				break;
			case 'M':
				b[i][0] = 'A'; b[i][1] = 'C';
				n[i] = 2;
				break;
			case 'D':
				b[i][0] = 'A'; b[i][1] = 'G'; b[i][1] = 'T';
				n[i] = 3;
				break;
			case 'H':
				b[i][0] = 'A'; b[i][1] = 'C'; b[i][1] = 'T';
				n[i] = 3;
				break;
		}
	}

	for (i = 0; i < n[0]; i++)
		for (j = 0; j < n[1]; j++)
			if (b[0][i] == b[1][j]) return true;

	return false;
}

bool Sequence::is_base_valid(TCHAR b)
{
	if (b == 'A' || b == 'T' || b == 'G' || b == 'C' ||
		b == 'W' || b == 'R' || b == 'Y' || b == 'S' || b == 'K' || b == 'M' || 
		b == 'D' || b == 'H' || b == 'N')
		return true;

	return false;
}

bool Sequence::is_seq_valid(const tstring &seq)
{
	if (seq.empty()) return false;

	int len = seq.length();
	for (int i = 0; i < len; i++) {
		if (!is_base_valid(seq[i])) return false;
	}
	return true;
}

TCHAR Sequence::comp_base(TCHAR b)
{
	switch (b) {
		case 'A': return 'T';
		case 'T': return 'A';
		case 'G': return 'C';
		case 'C': return 'G';
		case 'W': return 'W';
		case 'S': return 'S';
		case 'R': return 'Y';
		case 'Y': return 'R';
		case 'K': return 'M';
		case 'M': return 'K';
		case 'D': return 'H';
		case 'H': return 'D';
	}

	return b;
}

void Sequence::ini_seq()
{
	if (m_seq.empty()) {
		m_is_valid = false;
		return;
	}
	transform(m_seq.begin(), m_seq.end(), m_seq.begin(), ::toupper);
	m_is_valid = is_seq_valid(m_seq);
	m_dval = 0.0;
	m_ival = 0;

	return;
}

bool Sequence::find_best_match_offset(const Sequence &target, int &offset, int &matchnum)
{
	if (!m_is_valid || !target.m_is_valid ) return false;

	int _offset;
	int src_len = m_seq.length();
	int tar_len = target.m_seq.length();
	int min_offset = -1*src_len + 1;
	int max_offset = tar_len - 1;
	int max_match = 0;
	int offset_best = 0;
	
	for (_offset = min_offset; _offset <= max_offset; _offset++) {
		int match = 0;
		int tar_id, src_id;
		for (tar_id = _offset, src_id = 0; tar_id < tar_len && src_id < src_len; tar_id++, src_id++) {
			if (tar_id < 0) continue;
			if (is_base_match(m_seq[src_id], target.m_seq[tar_id])) match++;
		}
		
		if (match > max_match) {
			max_match = match;
			offset_best = _offset;
		}
		else if (match == max_match) {
			if (abs(_offset) > abs(offset_best)) offset_best = _offset;
		}
	}

	matchnum = max_match;
	offset = offset_best;

	return true;
}

bool Sequence::make_complement()
{
	if (m_seq.empty()) return false;
	int len = m_seq.length();
	tstring tmp = m_seq;
	for (int i = 0; i < len; i++) {
		tmp[len - 1 - i] = comp_base(m_seq[i]);
	}
	m_seq = tmp;

	return true;
}
