using System.Threading.Tasks;
using Orleans;
using EmployeeGrainInterfaces;
using System;
using System.Collections.Generic;

namespace EmployeeGrains
{
    /// <summary>
    /// Grain implementation class ManagerGrain.
    /// </summary>
    public class ManagerGrain : Grain, IManager
    {

        public override Task OnActivateAsync()
        {
            mMe = this.GrainFactory.GetGrain<IEmployee>(this.GetPrimaryKey());
            return base.OnActivateAsync();
        }

        //public Task AddDirectReport(IEmployee employee)
        //{
        //    mReports.Add(employee);
        //    employee.SetManager(this);
        //    employee.Greeting(mMe, "welcome to my team!");
        //    return Task.CompletedTask;
        //}

        public async Task AddDirectReport(IEmployee employee)
        {
            mReports.Add(employee);
            await employee.SetManager(this);
            await employee.Greeting(
                new GreetingData
                {
                    From = this.GetPrimaryKey(),
                    Message = "Welcome to my team!"
                }
                );
            //return Task.CompletedTask;
        }

        public Task<IEmployee> AsEmployee()
        {
            return Task.FromResult(mMe);
        }

        public Task<List<IEmployee>> GetDirectReports()
        {
            return Task.FromResult(mReports);
        }

        private IEmployee mMe;
        private List<IEmployee> mReports = new List<IEmployee>();
    }
}
