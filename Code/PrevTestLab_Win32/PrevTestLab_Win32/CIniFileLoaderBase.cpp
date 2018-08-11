#include "CIniFileLoaderBase.h"
using std::string;

// 텍스트의 한 행으로부터 특정 주석을 제거한다.
void RemoveCommentingFromLine(string& line) {
	// 특정 주석을 찾은 후 삭제한다.
	string::size_type idx = line.find('//');

	// 찾는 문자열이 존재한다면,
	if (idx != string::npos) {
		// '/'이 포함된 주석을 잘라낸다.
		line = line.substr(0, idx);	
	}
}

// 다음 유효한 파라미터를 지닌 텍스트 파일을 찾습니다. 몇 가지 주석들을 폐기하고
// 문자열로서 변수를 반환합니다.
std::string CIniFileLoaderBase::GetNextParameter() {
	// 이것은 다음 파라미터를 함축하는 문자열일 것입니다.
	std::string line;
	std::getline(file_, line);
	
	// 주석이라면 제거합니다.
	RemoveCommentingFromLine(line);

	// 만약 이것이 길이가 0인 문자열(주석)이면, 파일로부터
	// 다음 행을 가져옵니다.
	// Ref. 재귀적으로 작동합니다.
	// ?. 파일의 맨마지막 주석이라면, 무한루프에 걸리려나?
	if (line.length() == 0) {
		return GetNextParameter();
	}

	GetParameterValueAsString(line);

	return line;
}

// 텍스트의 한 행이 주어졌을 때, 이 함수는 파라미터 설명을 제거하고,
// std::string으로서 파라미터를 반환합니다.
void CIniFileLoaderBase::GetParameterValueAsString(std::string& line) {
	// 파라미터 설명의 시작부를 찾습니다.
	string::size_type begin_idx;
	string::size_type end_idx;

	// 몇 가지 구분 기호를 정의합니다.
	const string delims(" ;=,");

	// line에서 delims에 포함되지 않은 문자의 배열 인덱스를 가져옵니다.
	// Ex. Value 12 => result 0
	begin_idx = line.find_first_not_of(delims);

	// 파라미터 변수의 끝을 찾습니다.
	if (begin_idx != string::npos) {
		// begin_idx부터 delims이 속하는 맨 처음 인덱스를 가져옵니다.
		// Ex. Value 12 => result 5
		end_idx = line.find_first_of(delims, begin_idx);

		// 단어의 끝은 그 행의 끝입니다.
		if (end_idx == string::npos) {
			end_idx = line.length();
		}
	}

	// end_idx를 시작 인덱스로 하여 파라미터 변수의 시작을 찾습니다.
	// Ex. Value 12 => result 6
	begin_idx = line.find_first_not_of(delims, end_idx);

	// 파라미터 변수의 끝을 찾습니다.
	if (begin_idx != string::npos) {
		// begin_idx부터 delims에 속하는 맨 처음 인덱스를 가져옵니다.
		// Ex. Value 12 => result -1
		end_idx = line.find_first_of(delims, begin_idx);

		// 단어의 끝은 그 행의 끝입니다.
		if (end_idx == string::npos) {
			end_idx = line.length();
		}
	}

	line = line.substr(begin_idx, end_idx);
}

// 이것은 몇몇 주석을 무시하고 다음 행을 가져옵니다.
std::string CIniFileLoaderBase::GetNextToken() {
	// 몇몇 주석을 버립니다.
	while (current_line_.length() == 0) {
		std::getline(file_, current_line_);

		RemoveCommentingFromLine(current_line_);
	}

	// 파라미터 설명의 시작부를 찾습니다.
	string::size_type begin_idx;
	string::size_type end_idx;

	// 몇가지 구분기호를 정의합니다.
	const string delims(" ;=,");

	begin_idx = current_line_.find_first_not_of(delims);

	// 파라미터 설명의 끝부분을 찾습니다.
	if (begin_idx != string::npos) {
		end_idx = current_line_.find_first_of(delims, begin_idx);

		// 단어의 끝은 행의 끝입니다.
		if (end_idx == string::npos) {
			end_idx = current_line_.length();
		}
	}

	string s = current_line_.substr(begin_idx, end_idx);

	if (end_idx != current_line_.length()) {
		// 행으로부터 토큰을 제거합니다.
		current_line_ = current_line_.substr(end_idx + 1, current_line_.length());
	}
	else { current_line_ = ""; }

	return s;
}