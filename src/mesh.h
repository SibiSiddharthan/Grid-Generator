#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "2d.h"

enum class mesh_type
{
	_2d,
	_3d
};

class mesh
{
  private:
	std::vector<node> N;
	std::vector<edge> E;
	std::vector<mesh_triangle> T;
	std::vector<tetrahedron> TH;
	GLFWwindow *window = nullptr;
	mesh_type type;
	bool imported = false;
	bool ghost_generated = false;
	bool basic_generation = false;

	/*Creates a new triangle appends it the array and
		links it with its nodes	*/
	void make_triangle(const node_id , const node_id , const node_id , triangle_type = triangle_type::domain);

	void make_inside_edge(const node_id, const node_id,bool);

	inline const uint64_t number_of_nodes()
	{
		return N.size();
	}

	inline const uint64_t number_of_edges()
	{
		return E.size();
	}

	inline const uint64_t number_of_triangles()
	{
		return T.size();
	}

	//updated on 23/8/18
	//This function is only used by generate_mesh_basic
	inline bool left_test(const edge& e, const node& n)
	{
		return left_test_2d({ N[e.start].p, N[e.end].p}, n.p);
	}

	//updated on 23/8/18
	//This function is only used by generate_mesh_basic
	inline bool collinear_test(const edge& e, const node& n)
	{
		return !is_collinear({N[e.start].p, N[e.end].p},n.p);
	}

	//updated on 23/8/18
	//This function is only used by generate_mesh_basic
	inline bool intersection_test(const edge& e,const std::vector<edge>& E)
	{
		uint64_t count = 0;
		for (const edge& m_e : E)
		{
			if (do_they_intersect({N[m_e.start].p, N[m_e.end].p}, {N[e.start].p, N[e.end].p}))
			{
				++count;
				break;
			}
		}
		if (count == 0)
			return true;
		else
			return false;
	}

	inline const double edge_length(const edge& e)
	{
		return distance(N[e.start].p,N[e.end].p);
	}

	inline const double triangle_area(const mesh_triangle& m)
	{
		return area_of_triangle(N[m.a].p,N[m.b].p,N[m.c].p);
	}

	int64_t edge_exists(const std::vector<edge>& ,const edge& );
	void disable_common_node(const edge& ,const edge& );

	std::pair<node_id, node_id> corner_pos(const node &);
	pos generate_centroid(const std::vector<node>&);

	inline double triangle_min_angle(const triangle_id t_id)
	{
		return min_angle_of_triangle(N[T[t_id].a].p,N[T[t_id].b].p,N[T[t_id].c].p);
	}
	const node_id vertex_opposite_to_triangle_edge(const mesh_triangle& ,const edge & );

	pos generate_ghost_point(const triangle_id , const node_id);

	void triangle_node_change(const triangle_id,const node_id,const node_id);
	void edge_node_change(const edge_id,const node_id,const node_id);

	//Sweeps the entire mesh linking the triangles with it's nodes
	void node_triangle_share_sweep();
	void node_edge_share_sweep();
	void edge_triangle_share_sweep();
	void triangle_edge_share_sweep();
	void display_node(const std::vector<node> &m_N);
	void display_triangle(const std::vector<mesh_triangle> &m_T);
	void display_edge(const std::vector<edge> &m_E);
	void display_all_nodes();
	void display_all_edges();
	void display_all_triangles();
	inline void window_swap_buffers()
	{
		glfwSwapBuffers(window);
	}
	std::vector<float> pdata;
	void update_pdata();

  public:
	void init_2d();

	//imports from 2d object
	friend void import_2d(mesh &,const _2D_ &);

	//imports from 3d
	void import_3d();

	/*Generates a simple triangulation
		Details: Take a edge , take a node checks whether
		the triangle formed is valid i.e triangle lies within the domain,
		and triangle has a non zero area*/
	void generate_mesh_basic(bool debug = false);

	/*Inserts a node into the domain
		at the centroid of the polygon formed by
		the triangles that share a common boundary node*/
	void node_insertion();

	/*Forms 3 triangles by joining each of the triangle's edge with the 
		centroid of the triangle*/
	void refine_triangles();
	void refine_triangles_near_boundary(node_location);

	/*Swaps the edge of 2 adjacent triangles
		if the minimum angle increasese after swapping*/
	void edge_swap();

	/*Considers an inside node and changes it's
		position to the centroid of the polygon formed
		by all the triangles that share a common node inside the
		domain*/
	void centroid_shift();

	/*Generates a complete mesh
		Details: First generates basic mesh next does node insertion,
		equalize triangles and edge swap, finishes with centroid shift*/
	void generate_mesh_full();

	//Generates ghost triangles
	void generate_ghosts();

	inline void attach_window(GLFWwindow *_window)
	{
		window = _window;
	}

	//Displays the mesh onto the screen
	void display(bool inspect = false, bool swap_buffer = false);

	//Displays the number of nodes, triangle
	//and the average area of the triangles in the mesh
	void stats();

	//Returns the average area of all the triangles in the mesh
	double avg_area_of_triangles();

	//Returns the average area of the triangles which are near a boundary or a hole in the mesh
	double avg_area_of_triangles_near_boundary(node_location);

	
};





//updated on 23/8/18
//Returns the number of edges that have not been used by the basic mesh generator
//This function is only used by generate_mesh_basic
inline uint64_t number_of_unused_edges(const std::vector<edge>& E)
{
	uint64_t count = 0;
	for (const edge& e :E)
	{
		if (e.availability == true)
			count++;
	}
	return count;
}


