using Godot;
using System;

namespace Game
{
	public partial class HomeButton : Node
	{
		private PackedScene _mainMenuScene;

		public override void _Ready()
		{
			_mainMenuScene = GD.Load<PackedScene>(Config.HomeScenePath);
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
