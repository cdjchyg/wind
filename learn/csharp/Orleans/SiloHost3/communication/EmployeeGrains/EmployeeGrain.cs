using System;
using System.Threading.Tasks;
using EmployeeGrainInterfaces;
using Orleans;
using System.Threading;
using Orleans.Concurrency;

namespace EmployeeGrains
{
    [Reentrant]
    public class EmployeeGrain : Grain, IEmployee
    {
        public Task<int> GetLevel()
        {
            return Task.FromResult(mLevel);
        }

        public Task<IManager> GetManager()
        {
            return Task.FromResult(mManager);
        }

        public Task Promote(int newLevel)
        {
            mLevel = newLevel;
            return Task.CompletedTask;
        }

        public Task SetManager(IManager mgr)
        {
            mManager = mgr;
            return Task.CompletedTask;
        }


        //public Task Greeting(IEmployee from, string message)
        //{
        //    Thread.Sleep(2000);

        //    Console.WriteLine("{0} said: {1}", from.GetPrimaryKey().ToString(), message);

        //    return Task.CompletedTask;
        //}

        /*
         * Concurrency
         * 
         We've created a deadlock. Grain 0 sends a message to grain 1. 
         In that call grain 1 sends a message back to grain 0. However, 
         grain 0 can't process it because it's awaiting the first message, 
         so it gets queued. The await can't complete until the second message is returned, 
         so we've entered a state that we can't escape from. 
         Orleans waits for 30 seconds (10 minutes with the debugger), then kills the request.

         Orleans offers us a way to deal with this, by marking the grain [Reentrant], 
         which means that additional calls may be made while the grain is waiting for 
         a task to complete, resulting in interleaved execution.

        [Reentrant]
        public class Employee : Grain, IEmployee
        {
            ...
        }
        //http://www.cnblogs.com/liwt/p/orleans-singlethread.html
        [Reentrant] Attribute,�����������Ե�Grain�����ν��룬���˴�����������grain�ĵ��߳�ִ��ģ�͡�graincode��Ȼ�����ڵ��߳�ģ���£�ֻ���������󽻴�ִ�С�
        �����[Reentrant]֮�������ڿ�֮ǰA B��ʾ����
	        A����Ϣ��B���ȴ�B����Ӧ
	        B�յ���Ϣ���ַ���Ϣ��A��B��ʼ�ȴ�A����Ӧ
	        A��Ϊ�����[Reentrant] �����Խ��ܲ�����B��������Ϣ��A������Ϻ󷢻���Ӧ��B
	        B�յ���Ӧ������Լ��Ĵ�����̣�������Ӧ��A�������������
         */
        public async Task Greeting(GreetingData data)
        {
            Console.WriteLine("{0} said: {1}", data.From.ToString(), data.Message);
            if(data.Count >= 3)
            {
                return;
            }

            int i = 0;

            var fromGrain = GrainFactory.GetGrain<IEmployee>(data.From);
            await fromGrain.Greeting(
                new GreetingData {
                    From = this.GetPrimaryKey(),
                    Message = "Thanks!",
                    Count = data.Count + 1
                }
                );
            ++i;
            Console.WriteLine("local var i = " + i.ToString() + " " + data.From.ToString());
        }

        private int mLevel;
        private IManager mManager;
    }
}
