 

#ifndef __VS_UI_DESCRIPTOR_H__
#define __VS_UI_DESCRIPTOR_H__

#include "VS_UI_Base.h"

struct DESCRIBED_UNIT
{
	id_t		id;
	bool		bl_immediate; // immediate(true)/delay(false)
	void		(*fp_rect_calculator)(void (*fp_show)(Rect, void *, long, long), int, int, void *, long, long);
	void		(*fp_show)(Rect, void *, long, long);
};

//-----------------------------------------------------------------------------
// class DescriptorManager
//

//




//






//

//-----------------------------------------------------------------------------
class DescriptorManager : public SimpleDataList<DESCRIBED_UNIT *>
{
private:
	//
	// m_fp_show
	//


	//
	void							(*m_fp_show)(Rect, void *, long, long);

	struct FP_SHOW_PARAM
	{
		Rect						rect;
		void *					void_ptr;
		long						left;
		long						right;
	};

	FP_SHOW_PARAM				m_fp_show_param;

public:
	DescriptorManager();
	~DescriptorManager();

	void	Set(id_t id, int x, int y, void * void_ptr, long left=0, long right=0);
	//void	Unset();


	void	Unset(void* pPtr=NULL);	// by sigi

	void	Show();

	void	AddDescribedUnit(id_t id, void (*fp_rect_calculator)(void (*fp_show)(Rect, void *, long, long), int, int, void *, long, long), void (*fp_show)(Rect, void *, long, long), bool bl_immediate=true);
	void	RectCalculationFinished(void (*fp_show)(Rect, void *, long, long), Rect rect, void * void_ptr, long left, long right);
};

extern DescriptorManager	g_descriptor_manager;

#endif