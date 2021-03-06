

#pragma once

#ifndef _CLOCK_H_
#define _CLOCK_H_

// Library Includes
#include <chrono>
#include <vector>


// Prototypes
class CClock
{
	// Member Functions
public:
	CClock();
	~CClock();
	bool Initialise();
	void Process();
	float GetDeltaTick();

protected:

private:
	CClock(const CClock& _kr);
	CClock& operator= (const CClock& _kr);

	// Member Variables
public:

protected:
	long long m_fTimeElapsed;
	long long m_fDeltaTime;
	std::chrono::high_resolution_clock::time_point m_fLastTime;
	std::chrono::high_resolution_clock::time_point m_fCurrentTime;

	std::vector<double> m_vecTimeHistory;

	long long m_llNumCounts;

private:

};
#endif // 
