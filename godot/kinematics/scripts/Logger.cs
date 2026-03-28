using Godot;

namespace Game.Utils
{
    public class Logger
    {
        public static void PrintErr(string serviceName, string message)
        {
            GD.PrintErr($"{serviceName}: error: {message}");
        }
    }
}