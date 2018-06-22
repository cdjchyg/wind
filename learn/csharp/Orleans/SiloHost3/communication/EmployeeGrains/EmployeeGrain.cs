using System;
using System.Threading.Tasks;
using EmployeeGrainInterfaces;
using Orleans;
using System.Threading;

namespace EmployeeGrains
{
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

        public Task Greeting(IEmployee from, string message)
        {
            Thread.Sleep(2000);

            Console.WriteLine("{0} said: {1}", from.GetPrimaryKey().ToString(), message);

            return Task.CompletedTask;
        }

        private int mLevel;
        private IManager mManager;
    }
}
