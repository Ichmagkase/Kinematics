using Godot;

namespace Game.Menu
{
	public partial class StartButton : Button
	{
		PackedScene nextScene = GD.Load<PackedScene>(Config.GameScenePath);

		public void _OnPressed()
		{
			GetTree().ChangeSceneToPacked(nextScene);
		}
	}
}
