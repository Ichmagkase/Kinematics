using Godot;
using System;

namespace Game
{
	public partial class HomeButton : Node
	{
		private PackedScene _mainMenuScene;
		private GlobalConfig _config;

		public override void _Ready()
		{
			_config = GlobalConfig.Instance;
			_mainMenuScene = GD.Load<PackedScene>(_config.HomeScenePath);
		}

		public void _OnPressed()
		{
			var tree = GetTree();
			tree.Paused = false;
			GetOwner<Node>().QueueFree();			
			GetTree().ChangeSceneToPacked(_mainMenuScene);
		}
	}
}
