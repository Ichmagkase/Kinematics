using Godot;

namespace Game.Menu
{
	public partial class StartButton : Button
	{
		private GlobalConfig _config;
		PackedScene nextScene;

		public void _OnPressed()
		{

			_config = GlobalConfig.Instance;
			nextScene = GD.Load<PackedScene>(_config.GameScenePath);
			GetTree().ChangeSceneToPacked(nextScene);
		}
	}
}
