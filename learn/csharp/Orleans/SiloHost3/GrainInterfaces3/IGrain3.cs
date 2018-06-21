using System.Threading.Tasks;
using Orleans;

namespace GrainInterfaces3
{
    /// <summary>
    /// Grain interface IGrain1
    /// </summary>
    public interface IGrain3 : IGrainWithIntegerKey
    {
        Task<string> SayHello(string greeting);
    }
}
