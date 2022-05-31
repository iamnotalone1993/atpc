template<typename T>
class WFStackConsensus
{
private:
	const int	WIN	= 0;	// first thread
	const int	FAIL	= 1;	// second thread
	WFStack		stack;		// assume we already had WFStack
	T		proposal[2];

public:
	// initialize stack with two items
	WFStackConsensus()
	{
		stack.push(WIN);
		stack.push(LOSE);
	}

	// figure out which thread was first
	T decide(T val)
	{
		proposal[self] = val;
		if (stack.pop() == WIN)
			return proposal[self];
		else // if (stack.pop() == FAIL)
			return proposal[1-self];
	}
};
