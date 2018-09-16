#pragma once
//------------------------------------------------------------------------
//
//  Name: Smoother.h
//
//	Desc: ������ �������� ����ϴµ� ������ �ִ� ���ø� Ŭ�����̴�.
//		  �̰��� ���� �� ����, += �׸��� / �����ڵ��� �����ε�� Ÿ�Կ�
//		  ���ȴ�.
//
//		  ���� : ������ ����Ʈ ����� �ε巴�� �ϴµ� ���.
//
//  Author: Mat Buckland (fup@ai-junkie.com)
//
//------------------------------------------------------------------------
#include <vector>

template <class T>
class CSmoother
{
public:
	// �������� �ʱ�ȭ�ϱ� ���� �������ϴµ� ���Ǵ� ���� ���� ���� Ÿ�� ���ø�
	// �ѱ��.
	CSmoother(int SampleSize, T ZeroValue) :value_history_(SampleSize, ZeroValue),
		zero_value_(ZeroValue),
		next_update_slot_(0)
	{}

	// ���ο� ����� ��� ���� �� �ð����� �� �Լ��� �ֱ� ������ �ѱ��.
	// �׷��� �� �޼��尡 �ֱ� ���� ũ�� ������Ʈ�� ���� �����
	// ��ȯ�� ���̴�.
	T Update(const T& MostRecentValue)
	{
		// ���� ������ ������ �ֽ��� ������ ������.
		value_history_[next_update_slot_++] = MostRecentValue;

		// next_update_slot_�� ��ȯ�ϵ��� �Ѵ�.
		if (next_update_slot_ == value_history_.size()) next_update_slot_ = 0;

		// �����丮 ����Ʈ�� ����� ����Ѵ�.
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

	// �ε巴�� �� Ÿ���� �� ���Ϳ� ���� ����. �̰���
	// CVecter2D(0.0f, 0.0f)���� ���̴�.
	T             zero_value_;
};
