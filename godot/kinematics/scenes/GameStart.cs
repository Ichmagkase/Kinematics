using Godot;

namespace Game
{

	public partial class GameStart : Node2D
	{

		private Camera2D _currCamera;

		private void _CreatePlayers(int playerCount)
		{
			var currPlayer = GD.Load<PackedScene>(GlobalConfig.Instance.PlayerPath);
			var currPlayerPos = _currCamera.GetScreenCenterPosition();
			var leftStartingPos = new Vector2(currPlayerPos.X / -2, 50);
			var rightStartingPos = new Vector2(currPlayerPos.X / 2, 50);
			var flip = true;

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
		}
	}
}
