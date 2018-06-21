using System.Threading.Tasks;
using Orleans;

namespace Grains3
{
    /// <summary>
    /// Grain implementation class Grain3.
    /// </summary>
    public class Grain3 : Grain, GrainInterfaces3.IGrain3
    {
        public Task<string> SayHello()
        {
            return Task.FromResult("I'm From Grain3!");
        }
    }
}
