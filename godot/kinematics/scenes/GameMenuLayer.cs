using Godot;

namespace Game
{
	public partial class GameMenuLayer : Node
	{

		private PackedScene pauseMenu;
		private Control pauseMenuInstance;

		private void HandleGamePause()
		{
			if (pauseMenuInstance == null)
			{
				GetTree().Paused = true;
				pauseMenuInstance = pauseMenu.Instantiate<Control>();
				pauseMenuInstance.ProcessMode = ProcessModeEnum.WhenPaused;
				GetTree().Root.AddChild(pauseMenuInstance);
				pauseMenuInstance.ZIndex = 4096; // Appear above everyone
			}
			else
			{
				GetTree().Root.RemoveChild(pauseMenuInstance);
				pauseMenuInstance.QueueFree();
				pauseMenuInstance = null;
				GetTree().Paused = false;
			}

		}

		public override void _Ready()
		{
			
			pauseMenu = GD.Load<PackedScene>(Config.PauseScenePath);
			ProcessMode = ProcessModeEnum.Always;
		}

		public override void _Process(double delta)
		{
			if (Input.IsActionJustPressed(Config.PauseActionName))
			{
				GD.Print("Pausing!");
				HandleGamePause();
			}
			
		}
	}
}
