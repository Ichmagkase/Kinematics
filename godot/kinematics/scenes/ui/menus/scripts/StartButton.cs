using Godot;

namespace Game.Menu
{
	public partial class StartButton : Button
	{
		[Export]
		public PackedScene startScene;

		public void _OnPressed()
		{
			GetTree().ChangeSceneToPacked(startScene);
		}
	}
}
