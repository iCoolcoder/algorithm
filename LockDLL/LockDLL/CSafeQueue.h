
class CSafeQueue {
public:
	struct ELEMENT {
		int m_nThreadNum;
		int m_nRequestNum;
		// other data should go here
	};
	typedef ELEMENT* PELEMENT;
private:
	PELEMENT m_pElements;        // Array of elements to be processed
	int m_nMaxElements;          // Max number of elements in the array
	HANDLE m_h[2];               // Mutex & semaphore handles
	HANDLE m_hmtxQ;              // Reference to m_h[0]
	HANDLE m_hsemNumElements;    // Reference to m_h[1]
public:
	CSafeQueue(int nMaxElements);
	~CSafeQueue();

	BOOL Append(PELEMENT pElement, DWORD dwMillseconds);
	BOOL Remove(PELEMENT pElement, DWORD dwMillseconds);
};