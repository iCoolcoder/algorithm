class CQueue {
public:
	struct ELEMENT {
		int m_nThreadNum;
		int m_nRequestNum;
		// other element data should go here
	};
private:
	struct INNER_ELEMENT {
		int m_nStamp; // 0 means empty
		ELEMENT m_element;
	};
	typedef INNER_ELEMENT* PINNER_ELEMENT;

	PINNER_ELEMENT m_pElements; // Array of elements should be processed
	int m_nMaxElements;         // Maximum # of elements in the array
	int m_nCurrentStamp;        // Keep track of the # of added elements
private:
	int GetFreeSlot();
	int GetNextSlot(int nThreadNum);
public:
	CQueue(int nMaxElements);
	~CQueue();
	BOOL IsFull();
	BOOL IsEmpty(int nThreadNum);
	void AddElement(ELEMENT e);
	BOOL GetNewElement(int nThreadNum, ELEMENT& e);
};