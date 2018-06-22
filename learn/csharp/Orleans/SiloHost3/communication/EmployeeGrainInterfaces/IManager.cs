using System.Threading.Tasks;
using Orleans;
using System.Collections.Generic;

namespace EmployeeGrainInterfaces
{
    public interface IManager : IGrainWithGuidKey
    {
        Task<IEmployee> AsEmployee();
        Task<List<IEmployee>> GetDirectReports();
        Task AddDirectReport(IEmployee employee);
    }
}
