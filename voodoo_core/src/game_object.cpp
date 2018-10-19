// This file is part of Voodoo Engine.
//
// Voodoo Engine is free software : you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Voodoo Engine is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Foobar.  If not, see <https://www.gnu.org/licenses/>.

#include "../include/voodoo/game_object.h"
#include "../include/voodoo/component.h"
#include "../include/voodoo/logger.h"
#include "../include/voodoo/transform.h"

namespace voodoo {
GameObject::GameObject(std::string name)
    : name_(name),
      scene_(nullptr),
      parent_(nullptr),
      transform_(new Transform(std::shared_ptr<GameObject>(this))) {}

bool GameObject::Init(std::shared_ptr<Scene> scene) {
  scene_ = scene;

  for (auto component : GetComponents()) {
    if (!component->Init(shared_from_this())) {
      Log::Info("Failed to initialize component \"" + component->GetName() +
                "\"");
      return false;
    }
  }

  return true;
}

bool GameObject::Update() {
  for (auto component : GetComponents()) {
    if (!component->Update()) {
      Log::Info("Failed to update component \"" + component->GetName() +
                "\"");
      return false;
    }
  }

  return true;
}

std::vector<std::shared_ptr<Component>> GameObject::GetComponents() {
  std::vector<std::shared_ptr<Component>> result;

  for (auto it : components_) {
    result.push_back(it.second);
  }

  return result;
}

std::string GameObject::GetName() { return name_; }
std::shared_ptr<Scene> GameObject::GetScene() { return scene_; }
std::shared_ptr<GameObject> GameObject::GetParent() { return parent_; }
std::shared_ptr<Transform> GameObject::GetTransform() { return transform_; }
}  // namespace voodoo