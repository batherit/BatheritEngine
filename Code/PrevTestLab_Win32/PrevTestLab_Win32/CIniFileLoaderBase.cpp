#include "CIniFileLoaderBase.h"
using std::string;

// �ؽ�Ʈ�� �� �����κ��� Ư�� �ּ��� �����Ѵ�.
void RemoveCommentingFromLine(string& line) {
	// Ư�� �ּ��� ã�� �� �����Ѵ�.
	string::size_type idx = line.find('//');

	// ã�� ���ڿ��� �����Ѵٸ�,
	if (idx != string::npos) {
		// '/'�� ���Ե� �ּ��� �߶󳽴�.
		line = line.substr(0, idx);	
	}
}

// ���� ��ȿ�� �Ķ���͸� ���� �ؽ�Ʈ ������ ã���ϴ�. �� ���� �ּ����� ����ϰ�
// ���ڿ��μ� ������ ��ȯ�մϴ�.
std::string CIniFileLoaderBase::GetNextParameter() {
	// �̰��� ���� �Ķ���͸� �����ϴ� ���ڿ��� ���Դϴ�.
	std::string line;
	std::getline(file_, line);
	
	// �ּ��̶�� �����մϴ�.
	RemoveCommentingFromLine(line);

	// ���� �̰��� ���̰� 0�� ���ڿ�(�ּ�)�̸�, ���Ϸκ���
	// ���� ���� �����ɴϴ�.
	// Ref. ��������� �۵��մϴ�.
	// ?. ������ �Ǹ����� �ּ��̶��, ���ѷ����� �ɸ�����?
	if (line.length() == 0) {
		return GetNextParameter();
	}

	GetParameterValueAsString(line);

	return line;
}

// �ؽ�Ʈ�� �� ���� �־����� ��, �� �Լ��� �Ķ���� ������ �����ϰ�,
// std::string���μ� �Ķ���͸� ��ȯ�մϴ�.
void CIniFileLoaderBase::GetParameterValueAsString(std::string& line) {
	// �Ķ���� ������ ���ۺθ� ã���ϴ�.
	string::size_type begin_idx;
	string::size_type end_idx;

	// �� ���� ���� ��ȣ�� �����մϴ�.
	const string delims(" ;=,");

	// line���� delims�� ���Ե��� ���� ������ �迭 �ε����� �����ɴϴ�.
	// Ex. Value 12 => result 0
	begin_idx = line.find_first_not_of(delims);

	// �Ķ���� ������ ���� ã���ϴ�.
	if (begin_idx != string::npos) {
		// begin_idx���� delims�� ���ϴ� �� ó�� �ε����� �����ɴϴ�.
		// Ex. Value 12 => result 5
		end_idx = line.find_first_of(delims, begin_idx);

		// �ܾ��� ���� �� ���� ���Դϴ�.
		if (end_idx == string::npos) {
			end_idx = line.length();
		}
	}

	// end_idx�� ���� �ε����� �Ͽ� �Ķ���� ������ ������ ã���ϴ�.
	// Ex. Value 12 => result 6
	begin_idx = line.find_first_not_of(delims, end_idx);

	// �Ķ���� ������ ���� ã���ϴ�.
	if (begin_idx != string::npos) {
		// begin_idx���� delims�� ���ϴ� �� ó�� �ε����� �����ɴϴ�.
		// Ex. Value 12 => result -1
		end_idx = line.find_first_of(delims, begin_idx);

		// �ܾ��� ���� �� ���� ���Դϴ�.
		if (end_idx == string::npos) {
			end_idx = line.length();
		}
	}

	line = line.substr(begin_idx, end_idx);
}

// �̰��� ��� �ּ��� �����ϰ� ���� ���� �����ɴϴ�.
std::string CIniFileLoaderBase::GetNextToken() {
	// ��� �ּ��� �����ϴ�.
	while (current_line_.length() == 0) {
		std::getline(file_, current_line_);

		RemoveCommentingFromLine(current_line_);
	}

	// �Ķ���� ������ ���ۺθ� ã���ϴ�.
	string::size_type begin_idx;
	string::size_type end_idx;

	// ��� ���б�ȣ�� �����մϴ�.
	const string delims(" ;=,");

	begin_idx = current_line_.find_first_not_of(delims);

	// �Ķ���� ������ ���κ��� ã���ϴ�.
	if (begin_idx != string::npos) {
		end_idx = current_line_.find_first_of(delims, begin_idx);

		// �ܾ��� ���� ���� ���Դϴ�.
		if (end_idx == string::npos) {
			end_idx = current_line_.length();
		}
	}

	string s = current_line_.substr(begin_idx, end_idx);

	if (end_idx != current_line_.length()) {
		// �����κ��� ��ū�� �����մϴ�.
		current_line_ = current_line_.substr(end_idx + 1, current_line_.length());
	}
	else { current_line_ = ""; }

	return s;
}