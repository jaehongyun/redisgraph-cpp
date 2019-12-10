/**
* Copyright 2019 RRD Software Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
**/

#ifndef REDISGRAPH_CPP_GRAPH_H_
#define REDISGRAPH_CPP_GRAPH_H_
#include <string>
#include <future>
#include <memory>
#include <optional>
#include <unordered_map>
#include <utility>
#include <thread>
#include <connection_context.hpp>
#include <edge.hpp>
#include <node.hpp> 
#include <node_hash.hpp>
#include <result_view.hpp>

namespace redisgraph {

	

	/*
	* Create a graph interface with nodes that contains data of type T
	*/
	template <class T> class graph 
	{
		public:
			typedef typename std::unique_ptr<node<T>> unique_node;
			typedef typename std::hash<std::unique_ptr<redisgraph::node<T>>> node_hash;
			typedef typename std::unordered_map <unique_node, std::vector<edge<T>>, node_hash> adj_matrix;
			//
			/*
			* Constructor
			*/
			graph() = default;
			
			/**
		 	* Constructor
			* @param name  Name of the Graph
			* @param context Redis Configuration
			*/

			explicit graph(const std::string& name, const redisgraph::connection_context& context): name_(name), context_(context)
			{
				adj_matrix map;
				graph_ = std::make_unique<adj_matrix>(map);
				started_ = false;
			} 
			/**
			* Copy constructor. The graph is not copyable.
			* @param graph Graph to be copied.
			*/
			graph(const redisgraph::graph<T>& graph) = delete;
			/**
			* Copy assignment operator. The graph is not copyable
			* @param Graph to be assigned.
			*/
			const redisgraph::graph<T>& operator=(const redisgraph::graph<T>& graph) = delete;
			
			/** 
			 * Move semantics 
			 **/
			explicit graph(graph&& g)
			{
				g.shutdown();
				graph_ = std::move(g.graph_);
				context_ = std::move(g.context_);
				name_ = std::move(g.name_);
				num_nodes_ = std::move(g.num_nodes_);
				graph_->start();
			}
			~graph() {
				if (started_)
				{
				    shutdown();
				}
			}

			/// Getter and setter		
			/**
			* Get the name of the graph
			*/
			std::string name() const { return name_; }
			/**
			* Get the number of nodes
			*/
			int num_nodes() const { return  num_nodes_; }
			
			/**
			 * Add a node and label to the node.
			 * @return A copy of the added node if added with success.
			 */
			std::optional<node<T>> add_node(const std::string& name, T data) noexcept
			{
				node<T> current_node{ name, data };
				auto node = graph_->find(current_node);
				if (node != node.end())
				{
					graph_->insert(std::make_pair(std::make_unique<node>(current_node), std::vector()));
					return current_node;
				}
				return std::nullopt;
			}
			/*
			*  Remove a node 
			*  @return A copy of the removed node
			*/
			std::optional<node<T>> remove_node(const std::string& name)
			{

			}
			/**
			* Add a new edge with a relation from the source node to the destination node
			*/
			std::optional<edge<T>> add_edge(const std::string& relation, const node<T>& source, const node<T>& dest) noexcept
			{
					// find in the node relation if from source there is a node to destination.
				auto currentNode = graph_->find(source);
				bool existEdge = false;
				if (currentNode != currentNode.end())
				{
					// found.
					for (const auto& e : currentNode->second)
					{
						if (find_direct_connection(source, dest ,e))
						{
							existEdge = true;
						}
					}
					if (!existEdge)
					{
						// we can add
						auto currentEdge = edge{ relation, source.id(), dest.id() };
						currentNode->second.push_back(currentEdge);
						return currentEdge;
					}
				}
				// in all cases return edge
				return std::nullopt;
			}
			/*
			* startup the connection pool to redis 
			*/
			void start()
			{
				started_ = true;
			}
			/**
			 *  shutdown the connection pool to redis 
			 */
			void shutdown()
			{
				started_ = false;
			}
			/**
			 * Query asynchronously to redis graph
			 * @param query OpenCypher Query
			 */
			std::future<redisgraph::result_view> query_async(const std::string& query)
			{
				std::packaged_task<redisgraph::result_view()> task([]() { return result_view(); }); // wrap the function
				return task.get_future();  // get 
			}

			/**
			 * Commit the current graph structure in memory to redis graph creating a graph 
			 * @throw When there are connection problems
			 * @return Return a future to the result (true when it has succeded)
			 */
			std::future<bool> commit_async()
			{
				std::packaged_task<bool()> task([]() { return false; }); // wrap the function
				return task.get_future();  // get 
			}
			
			/**
			* Commit the current graph structure in memory and flush its content
			*/
			bool flush()
			{
				return false;
			}			
		private:
			void init_connection(const redisgraph::connection_context& context)
			{

			}
			bool find_direct_connection(const node<T>& source, const node<T>& dest, const edge<T>& e)
			{
				auto sourceId = source.id();
				auto destinationId = dest.id();
				return (e.source() == sourceId) && (e.dest() == destinationId)
			}
			std::string name_;
			int num_nodes_;
			bool started_;
			redisgraph::connection_context context_;
			std::unique_ptr<adj_matrix> graph_;
			
	};
	template <typename T> graph<T> make_graph(const std::string& graph_name,
		const std::string& host = "127.0.0.1",
		const unsigned int& port = 6379,
		const unsigned int& concurrency = 4)
	{
		connection_context ctx{ host, port, concurrency };
		graph<T> g(graph_name, ctx);
		return g;
	};
}
#endif /* REDISGRAPH_CPP_GRAPH_H_ */