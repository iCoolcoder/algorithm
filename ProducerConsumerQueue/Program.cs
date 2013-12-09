
namespace ProducerConsumerQueue
{
    class Program
    {
        static void Main(string[] args)
        {
            using (ProducerConsumerQueue q = new ProducerConsumerQueue())
            {
                q.EnqueueTask("Hello");
                for(int i = 0; i < 10; i++) q.EnqueueTask("Say " + i);
                q.EnqueueTask("GoodBye!");
            }
        }
    }
}
