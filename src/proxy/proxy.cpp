#include "proxy.h"

void to_json(json& j, const V2rayTransport& v)
{
    j = json{ {"type", v.type_}, {"path", v.path_}, {"headers", v.headers_},
{"max_early_data", v.max_early_data_}, {"early_data_header_name", v.early_data_header_name_}
    };
}

void to_json(json& j, const Headers& h)
{
    j = json{ {"host", h.host_} };
}

void to_json(json& j, const Vmess& v)
{
    j = json{ {"type", v.type_}, {"tag", v.tag_}, {"server", v.server_},
    {"server_port", v.server_port_}, {"uuid", v.uuid_}, {"security", v.security_},
    {"alter_id", v.alter_id_}, {"global_padding", v.global_padding_}, {"authenticated_length", v.authenticated_length_},
    {"tls", json::object()}, {"packet_encoding", v.packet_encoding_}, {"transport", *v.transport_}
    };
    if (!v.network_.empty())
        j.push_back({ "network", v.network_ });
}