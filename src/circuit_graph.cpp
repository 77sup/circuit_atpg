#include "../include/circuit_graph.h"
#include "log.h"
#include <iostream>
#include <sstream>
#include <map>
#include <set>

const char *make_gate_name(Gate::Type type)
{
	int flag = -1;
	switch (type)
	{
	case Gate::Type::Input:
		return "Input";
	case Gate::Type::And:
		return "AND";
	case Gate::Type::Nand:
		return "NAND";
	case Gate::Type::Not:
		return "NOT";
	case Gate::Type::Or:
		return "OR";
	case Gate::Type::Nor:
		return "NOR";
	case Gate::Type::Xor:
		return "XOR";
	case Gate::Type::Xnor:
		return "XNOR";
	case Gate::Type::Buff:
		return "BUFF";
	case Gate::Type::Undefined:
		return "UNDEFINED";
	}
	assert(false);
	return "???";
}

Gate::Gate(Gate::Type type, Line *output, std::vector<Line *> &&inputs) : m_type(type), m_inputs(inputs), m_output(output)
{
}
Gate::Gate(std::vector<Line *> &&inputs) : m_inputs(inputs)
{
	m_type = Gate::Type::Or;
}
Gate::Gate(Line *output)
{
	this->m_type = Gate::Type::Input;
	this->m_output = output;
}

std::string Gate::get_str() const
{
	std::stringstream ss;
	ss << m_output->name << " = " << make_gate_name(m_type) << "(";
	for (auto it = m_inputs.begin(); it != m_inputs.end() - 1; ++it)
	{
		ss << (*it)->name << ", ";
	}
	ss << m_inputs.back()->name;
	ss << ")";
	return ss.str();
}

Line *CircuitGraph::add_input(const std::string &name)
{
	Line *p_line = ensure_line(name);

	assert(p_line);

	m_inputs.push_back(p_line);
	m_gates.emplace_back(p_line);
	Gate &gate = m_gates.back();
	p_line->source = &gate;
	
	return p_line;
}

Line *CircuitGraph::add_output(const std::string &name)
{
	Line *p_line = ensure_line(name);

	assert(p_line);

	if (!p_line->is_output)
	{
		p_line->is_output = true;
		m_outputs.push_back(p_line);
	}

	return p_line;
}

Gate *CircuitGraph::add_gate(Gate::Type type, const std::vector<std::string> &input_names, const std::string &output_name)
{

	std::vector<Line *> inputs;
	for (size_t i = 0; i < input_names.size(); ++i)
	{
		Line *p_input = ensure_line(input_names.at(i));
		inputs.push_back(p_input);
	}

	Line *p_output = ensure_line(output_name);

	m_gates.emplace_back(type, p_output, std::move(inputs));
	Gate &gate = m_gates.back();

	p_output->source = &gate;

	for (size_t i = 0; i < gate.get_inputs().size(); ++i)
	{
		gate.get_inputs().at(i)->connect_as_input(&gate);
	}

	// Gate validation
	switch (gate.type())
	{
	case Gate::Type::And:
	case Gate::Type::Nand:
	case Gate::Type::Or:
	case Gate::Type::Nor:
		assert(gate.inputs().size() >= 2);
		break;
	case Gate::Type::Xor:
	case Gate::Type::Xnor:
		assert(gate.inputs().size() == 2);
		break;
	case Gate::Type::Not:
	case Gate::Type::Buff:
		assert(gate.inputs().size() == 1);
		break;
	default:
		assert(false);
	}
	return &gate;
}

Gate *CircuitGraph::add_learnt_gate(std::vector<Line *> input_names, std::vector<int> &inputs_polarity)
{
	Gate::Type type = Gate::Type::Or;
	m_gates.emplace_back(std::move(input_names));
	Gate &gate = m_gates.back();
	
	gate.change_learnt_gate(true);
	gate.change_inputs_polarity(inputs_polarity);
	gate.get_pointers().first = gate.get_inputs()[0]->id;
	gate.get_pointers().second = gate.get_inputs()[1]->id;

	for (size_t i = 0; i < gate.get_inputs().size(); ++i)
	{
		gate.get_inputs().at(i)->connect_as_input(&gate);
	}
	return &gate;
}

Line &CircuitGraph::line(const int &id)
{
	if(id < m_lines.size())
		return m_lines[id];
	else
		std::cout << "id > m_lines.size()" <<std::endl;
	return m_lines[0];
}
const Line &CircuitGraph::get_line(const int &id) const
{
	if(id < m_lines.size())
		return m_lines[id];
	else
		std::cout << "id > m_lines.size()" <<std::endl;
	return m_lines[0];
}

const std::vector<Line *> &CircuitGraph::get_inputs() const
{
	return m_inputs;
}
const std::vector<Line *> &CircuitGraph::get_outputs() const
{
	return m_outputs;
}

const std::deque<Gate> &CircuitGraph::get_gates() const
{
	return m_gates;
}
std::deque<Gate> &CircuitGraph::get_gates()
{
	return m_gates;
	
}
const std::deque<Line> &CircuitGraph::get_lines() const
{
	return m_lines;
}


void CircuitGraph::get_graph_stats() const
{
	std::stringstream ss;

	ss << "# " << m_inputs.size() << " input" << (m_inputs.size() > 1 ? "s" : "") << "\n";
	ss << "# " << m_outputs.size() << " output" << (m_outputs.size() > 1 ? "s" : "") << "\n";
	ss << "# " << m_lines.size() << " line" << (m_lines.size() > 1 ? "s" : "") << "\n";
	ss << "# " << m_gates.size() << " gate" << (m_gates.size() > 1 ? "s" : "") << ":\n";

	std::map<Gate::Type, size_t> gate_types;
	for (const auto &gate : m_gates)
	{
		++gate_types[gate.get_type()];
	}

	for (const auto &type_count_pair : gate_types)
	{
		ss << "#     ";
		ss << type_count_pair.second << " ";
		ss << make_gate_name(type_count_pair.first);
		ss << "\n";
	}
	for (int i = 0; i < m_gates.size(); i++)
	{
		std::cout << m_gates[i].get_str() << std::endl;
	}
	std::cout << ss.str() << std::endl;
}

Line *CircuitGraph::ensure_line(const std::string &name)
{
	auto it = m_name_to_line.find(name);

	if (it != m_name_to_line.end())
	{
		return it->second;
	}

	m_lines.emplace_back(name, false);
	Line &line = m_lines.back();

	line.id = m_lines.size() - 1;

	m_name_to_line[name] = &line;

	it = m_name_to_line.find(name);

	return &line;
}
