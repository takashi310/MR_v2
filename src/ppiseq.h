
#ifndef _PPISEQ_H_
#define _PPISEQ_H_

#include <stdio.h>
#include <tchar.h>
#include <ctype.h>
#include <conio.h>
#include <locale>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

typedef std::basic_string<TCHAR> tstring;

typedef std::basic_ostream<TCHAR> tostream; // �W���o�̓X�g���[��
typedef std::basic_istream<TCHAR> tistream; // �W�����̓X�g���[��
typedef std::basic_stringstream<_TCHAR> tstringstream;  // ������o�̓X�g���[��
typedef std::basic_ostringstream<TCHAR> tostringstream; // ������o�̓X�g���[���i���`�i�[�p�o�b�t�@�j
typedef std::basic_istringstream<TCHAR> tistringstream; // ��������̓X�g���[���i���`�擾�p�o�b�t�@�j
typedef std::basic_ifstream<TCHAR> tifstream; // �t�@�C�����̓X�g���[��
typedef std::basic_ofstream<TCHAR> tofstream; // �t�@�C���o�̓X�g���[��
typedef std::basic_fstream<TCHAR> tfstream; // �t�@�C���X�g���[��

// �W���o�͂̐؂�ւ��}�N��
#if defined(_UNICODE) || defined(UNICODE)
#define tcin wcin // ����
#define tcout wcout // �o��
#define tcerr wcerr // �G���[
#define tclog wclog // ���O
#else
#define tcin cin // ����
#define tcout cout // �o��
#define tcerr cerr // �G���[
#define tclog clog // ���O
#endif


class Sequence {
public:
	Sequence()
	{
		m_seq = _T("");
		ini_seq();
	}
	Sequence(const tstring &seq)
	{
		m_seq = seq;
		ini_seq();
	}
	Sequence(const Sequence &s)
	{
		m_seq = s.m_seq;
		m_is_valid = s.m_is_valid;
		m_dval = s.m_dval;
		m_ival = s.m_ival;
	}
	~Sequence(){};
	static bool is_base_match(TCHAR b1, TCHAR b2);
	static bool is_base_valid(TCHAR b);
	static bool is_seq_valid(const tstring &seq);
	static TCHAR comp_base(TCHAR b);
	static bool less_dval(const Sequence& rLeft, const Sequence& rRight) { return rLeft.m_dval < rRight.m_dval; }
	static bool high_dval(const Sequence& rLeft, const Sequence& rRight) { return rLeft.m_dval > rRight.m_dval; }
	static bool less_ival(const Sequence& rLeft, const Sequence& rRight) { return rLeft.m_ival < rRight.m_ival; }
	static bool high_ival(const Sequence& rLeft, const Sequence& rRight) { return rLeft.m_ival > rRight.m_ival; }
	bool find_best_match_offset(const Sequence &target, int &offset, int &matchnum);
	bool make_complement();
	bool is_valid() {return m_is_valid;}

	void set_sequence(const tstring &seq) {m_seq = seq; ini_seq();}
	tstring get_sequence() {return m_seq;}
	void set_dval(double val) {m_dval = val;}
	double get_dval() {return m_dval;}
	void set_ival(int val) {m_ival = val;}
	double get_ival() {return m_ival;}

private:
	tstring m_seq;
	double m_dval;
	int m_ival;
	bool m_is_valid;
	void ini_seq();
};

#endif //_PPISEQ_H_