using Godot;

namespace Game.Menu
{
	public partial class RestartButton : Button
	{
		[Export]
		public PackedScene startScene;

		public void _OnPressed()
		{
			GetTree().ChangeSceneToPacked(startScene);
		}
	}
}
