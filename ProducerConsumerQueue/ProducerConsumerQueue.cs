using System;
using System.Collections.Generic;
using System.Threading;

namespace ProducerConsumerQueue
{
    class ProducerConsumerQueue : IDisposable
    {
        private EventWaitHandle _wh = new AutoResetEvent(false);
        private Thread _worker;
        private readonly object _locker = new object();
        private Queue<string> _tasks = new Queue<string>();

        public ProducerConsumerQueue()
        {
            _worker = new Thread(Work);
            _worker.Start();
        }

        public void EnqueueTask(string task)
        {
            lock (_locker)
            {
                _tasks.Enqueue(task);
                _wh.Set();
            }
        }

        public void Dispose()
        {
            EnqueueTask(null);
            _worker.Join();
            _wh.Close();
        }

        void Work()
        {
            while (true)
            {
                string task = null;
                lock (_locker)
                {
                    if (_tasks.Count > 0)
                    {
                        task = _tasks.Dequeue();
                        if(task == null) return;
                    }
                }
                if (task != null)
                {
                    Console.WriteLine("Performing task: " + task);
                    Thread.Sleep(1000); // simulate work ...
                }
                else
                {
                    _wh.WaitOne();
                }
            }
        }
    }
}
