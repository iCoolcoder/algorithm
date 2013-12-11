using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Cryptography.X509Certificates;
using System.Text;
using System.Threading;

namespace ProducerConsumerQueue
{
    class PCQueue
    {
        private readonly object _locker = new object();
        private Thread[] _workers;
        private Queue<Action> _itemQ = new Queue<Action>();

        public PCQueue(int workerCount)
        {
            _workers = new Thread[workerCount];
            
            //create and start a separate thread for each worker
            for (int i = 0; i < workerCount; i++)
            {
                (_workers[i] = new Thread(Consume)).Start();
            }
        }

        public void ShutDown(bool waitForWorkers)
        {
            // Enqueue one null item per worker to make each exit.
            foreach (var thread in _workers)
            {
                EnqueueItem(null);
            }

            // Wait for workers to finish
            if(waitForWorkers)
                foreach (var thread in _workers)
                {
                    thread.Join();
                }
        }

        public void EnqueueItem(Action item)
        {
            lock (_locker)
            {
                _itemQ.Enqueue(item);
                Monitor.Pulse(_locker);
            }
        }

        private void Consume()
        {
            while (true)
            {
                Action item;
                lock (_locker)
                {
                    while (_itemQ.Count == 0) Monitor.Wait(_locker);
                    item = _itemQ.Dequeue();
                }
                if(item == null) return;
                item();
            }
        }
    }
}
