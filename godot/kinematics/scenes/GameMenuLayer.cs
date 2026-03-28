using Godot;

namespace Game
{
	public partial class GameMenuLayer : Node
	{

		private PackedScene pauseMenu;
		private Control pauseMenuInstance;
		private GlobalConfig _config;

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
			
			_config = _config = GlobalConfig.Instance;
			pauseMenu = GD.Load<PackedScene>(_config.PauseScenePath);
			ProcessMode = ProcessModeEnum.Always;
		}

		public override void _Process(double delta)
		{
			if (Input.IsActionJustPressed(_config.PauseActionName))
			{
				HandleGamePause();
			}
			
		}
	}
}
