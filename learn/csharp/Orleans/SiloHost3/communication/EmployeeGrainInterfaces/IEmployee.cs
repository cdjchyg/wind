using System.Threading.Tasks;
using Orleans;

namespace EmployeeGrainInterfaces
{
    /// <summary>
    /// Grain interface IGrain1
    /// </summary>
    public interface IEmployee : IGrainWithGuidKey
    {
        Task<int> GetLevel();
        Task Promote(int newLevel);

        Task<IManager> GetManager();
        Task SetManager(IManager mgr);

        Task Greeting(IEmployee from, string message);
    }
}
