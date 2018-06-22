using System.Threading.Tasks;
using Orleans;

namespace Stock
{
    /// <summary>
    /// Grain interface IGrain1
    /// </summary>
    public interface IStockGrain : IGrainWithStringKey
    {
        Task<string> GetPrice();
    }
}
