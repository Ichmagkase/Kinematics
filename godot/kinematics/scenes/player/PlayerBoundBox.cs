using Godot;
using System.Collections.Generic;

namespace Game.Player
{
	public partial class PlayerBoundBox : CollisionShape2D
	{
		private PlayerConfig _playerConfig;
		private PlayerSprite _animatedSprite;
		private RectangleShape2D _rect;
		private string _lastAnimation = "";
		private bool _lastFlipH;
		private Dictionary<string, Vector2> _animationSizes;
		private Dictionary<string, Vector2> _offsetSizes;
		
		public override void _Ready()
		{
			_playerConfig = GetParent().GetNode<PlayerConfig>("PlayerConfig");
			_animationSizes = _playerConfig.AnimationNameToBoundingBoxSize;
			_offsetSizes = _playerConfig.AnimationNameToBoundingBoxOffsetSize;
			_animatedSprite = GetParent().GetNode<PlayerSprite>("AnimatedSprite2D");
			_lastFlipH = _animatedSprite.FlipH;
			_rect = (RectangleShape2D)Shape;

			if (_animatedSprite == null)
				GD.PrintErr("PlayerBoundBox: Could not find AnimatedSprite2D on parent");

			if (_rect == null)
				GD.PrintErr("PlayerBoundBox: CollisionShape2D is not using a RectangleShape2D");
		}

		public override void _Process(double delta)
		{
			if (_animatedSprite == null || _rect == null) return;

			string currentAnimation = _animatedSprite.GetCurrentAnimationName();
			bool currentFlipH = _animatedSprite.FlipH;

			if (currentAnimation != _lastAnimation || currentFlipH != _lastFlipH)
			{
				_lastAnimation = currentAnimation;
				_lastFlipH = currentFlipH;
				UpdateBoundingBox(currentAnimation, currentFlipH);
			}

		}

		private void UpdateBoundingBox(string animation, bool isFlipped)
		{
			Vector2 newSize = _animationSizes.TryGetValue(animation, out Vector2 size)
				? size
				: _animationSizes.TryGetValue("default", out Vector2 def)
					? def : Vector2.Zero;

			string flipKey = animation + "_flipH";
			Vector2 finalOffset;

			if (isFlipped && _offsetSizes.TryGetValue(flipKey, out Vector2 flipAnimOffset))
			{
				finalOffset = flipAnimOffset;
			}
			else
			{
				Vector2 baseOffset = _offsetSizes.TryGetValue(animation, out Vector2 offset)
					? offset
					: _offsetSizes.TryGetValue("default", out Vector2 defOffset)
						? defOffset : Vector2.Zero;

				finalOffset = baseOffset;
				if (isFlipped && _offsetSizes.TryGetValue("flipH", out Vector2 flipHOffset))
				{
					finalOffset += flipHOffset;
				}
			}

			_rect.Size = newSize;
			Position = finalOffset;
		}
	}
}
