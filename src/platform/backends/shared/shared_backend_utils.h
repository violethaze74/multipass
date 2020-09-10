/*
 * Copyright (C) 2020 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef MULTIPASS_SHARED_BACKEND_UTILS_H
#define MULTIPASS_SHARED_BACKEND_UTILS_H

#include <multipass/utils.h>
#include <multipass/virtual_machine.h>

#include <chrono>
#include <string>

namespace multipass
{
namespace backend
{
template <typename Callable>
std::string ip_address_for(VirtualMachine* virtual_machine, Callable&& get_ip, std::chrono::milliseconds timeout)
{
    if (!virtual_machine->ip)
    {
        auto action = [virtual_machine, get_ip] {
            virtual_machine->ensure_vm_is_running();
            auto result = get_ip();
            if (result)
            {
                virtual_machine->ip.emplace(*result);
                return utils::TimeoutAction::done;
            }
            else
            {
                return utils::TimeoutAction::retry;
            }
        };

        auto on_timeout = [virtual_machine] {
            virtual_machine->state = VirtualMachine::State::unknown;
            throw std::runtime_error("failed to determine IP address");
        };

        utils::try_action_for(on_timeout, timeout, action);
    }

    return virtual_machine->ip->as_string();
}
} // namespace backend
} // namespace multipass

#endif // MULTIPASS_SHARED_BACKEND_UTILS_H
