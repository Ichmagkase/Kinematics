using Godot;
using System.Collections.Generic;

namespace Game
{

	public partial class GameStart : Node2D
	{

		private Camera2D _currCamera;
		private List<Player.Player> _playerNodes;

		private void _CreatePlayerHUDs()
        {
			var playerHudContainer = GetNode<VBoxContainer>("PlayerHUDContainer");
			if (playerHudContainer == null)
            {
                GD.PrintErr("failed to add player huds, no VBoxContainer node 'PlayerHUDContainer' found");
				return;
            }

            foreach (Player.Player p in _playerNodes)
            {
				// Give player the progress bar
				var playerHudInstance = GD.Load<PackedScene>(GlobalConfig.Instance.PlayerHudPath).Instantiate<Control>();
				if (playerHudInstance == null)
                {
                    GD.PrintErr($"failed to instantiate player hud for player {p.PlayerId}");
					continue;
                }
				var progressBar = playerHudInstance.GetNode<ProgressBar>("HealthBarContainer/ProgressBar");
				if (progressBar == null)
                {
                    GD.PrintErr($"failed to get ProgressBar from player hud for player {p.PlayerId}");
					continue;
                }
                p.HealthBar = progressBar;
				p.HealthBarLabel = progressBar.GetNode<Label>("HealthPercentageContainer/HealthPercentage");

				// Assign that progress bar name to be the player
				var playerName = playerHudInstance.GetNode<Label>("HealthBarContainer/PlayerName");
				if (playerName == null)
                {
                    GD.PrintErr($"failed to get PlayerName label from player hud for player {p.PlayerId}");
					continue;
                }
				playerName.Text = $"Player {p.PlayerId}";

				playerHudContainer.AddChild(playerHudInstance);

            }
        }

		private void _CreatePlayers(int playerCount)
		{
			var currPlayer = GD.Load<PackedScene>(GlobalConfig.Instance.PlayerPath);
			var currPlayerPos = _currCamera.GetScreenCenterPosition();
			var leftStartingPos = new Vector2(currPlayerPos.X / -2, 50);
			var rightStartingPos = new Vector2(currPlayerPos.X / 2, 50);
			var flip = true;

			_playerNodes = new List<Player.Player>();
			for (int i = 1; i <= playerCount; i++)
			{
				var instance = currPlayer.Instantiate<Player.Player>();
				instance.PlayerId = i;
				AddChild(instance);
				if (flip) 
				{ 
					instance.GlobalPosition = currPlayerPos + leftStartingPos; 
					flip = false; 
				}
				else  
				{ 
					instance.GlobalPosition = currPlayerPos + rightStartingPos;
					flip = true; 
				}
				_playerNodes.Add(instance);

				// Update label of player
				var playerName = instance.GetNode<Label>("PlayerNameContainer/PlayerName");
				playerName.Text = $"Player {instance.PlayerId}";
			}
		}

		private void _SetupPlayerSignals()
		{
			foreach (Player.Player p in _playerNodes)
			{
				switch (p.PlayerId)
				{
					case 1:
						EventBus.Instance.PlayerOneAction += p.HandleActionSignal;
						break;
					case 2:
						EventBus.Instance.PlayerTwoAction += p.HandleActionSignal;
						break;
					default:
						GD.PrintErr($"failed to assign singal to player with ID: {p.PlayerId}, no known signal for this id");
						break;
				}
			}
		}

		private void _TeardownPlayerSignals()
		{
			foreach (Player.Player p in _playerNodes)
			{
				switch (p.PlayerId)
				{
					case 1:
						EventBus.Instance.PlayerOneAction -= p.HandleActionSignal;
						break;
					case 2:
						EventBus.Instance.PlayerTwoAction -= p.HandleActionSignal;
						break;
				}
			}
		}

		public override void _Ready()
		{
			string cameraNodeName = "Camera2D";

			_currCamera = GetNode<Camera2D>(cameraNodeName);
			if (_currCamera == null)
			{
				GD.PrintErr($"failed to find child node '${cameraNodeName}', No place to put players");
				return;
			}

			int count = GlobalConfig.Instance.PlayerCount;
			_CreatePlayers(count);
			_CreatePlayerHUDs();
			_SetupPlayerSignals();
		}

		public override void _ExitTree()
		{
			_TeardownPlayerSignals();
		}
	}
}
