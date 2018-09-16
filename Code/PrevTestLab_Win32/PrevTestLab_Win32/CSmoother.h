#pragma once
//------------------------------------------------------------------------
//
//  Name: Smoother.h
//
//	Desc: 나열된 변수들을 평균하는데 도움을 주는 템플릿 클래스이다.
//		  이것은 오직 영 벡터, += 그리고 / 연산자들이 오버로드된 타입에
//		  사용된다.
//
//		  예시 : 프레임 레이트 계산을 부드럽게 하는데 사용.
//
//  Author: Mat Buckland (fup@ai-junkie.com)
//
//------------------------------------------------------------------------
#include <vector>

template <class T>
class CSmoother
{
public:
	// 스무서를 초기화하기 위해 스무싱하는데 사용되는 샘플 수와 제로 타입 예시를
	// 넘긴다.
	CSmoother(int SampleSize, T ZeroValue) :value_history_(SampleSize, ZeroValue),
		zero_value_(ZeroValue),
		next_update_slot_(0)
	{}

	// 새로운 평균을 얻고 싶은 매 시간마다 이 함수에 최근 변수를 넘긴다.
	// 그러면 이 메서드가 최근 샘플 크기 업데이트를 통해 평균을
	// 반환할 것이다.
	T Update(const T& MostRecentValue)
	{
		// 가장 오래된 변수를 최신의 것으로 덮어씌운다.
		value_history_[next_update_slot_++] = MostRecentValue;

		// next_update_slot_이 순환하도록 한다.
		if (next_update_slot_ == value_history_.size()) next_update_slot_ = 0;

		// 히스토리 리스트의 평균을 계산한다.
		T sum = zero_value_;

		typename std::vector<T>::iterator it = value_history_.begin();

		for (it; it != value_history_.end(); ++it)
		{
			sum += *it;
		}

		return sum / (float)value_history_.size();
	}

private:
	std::vector<T>  value_history_;

	int           next_update_slot_;

	// 부드럽게 할 타입의 영 벡터에 대한 예시. 이것은
	// CVecter2D(0.0f, 0.0f)같은 것이다.
	T             zero_value_;
};
