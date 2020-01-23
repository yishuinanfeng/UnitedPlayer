package com.man.manchesterunitedplayer

import android.content.Context
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import androidx.recyclerview.widget.RecyclerView
import com.man.manchesterunitedplayer.bean.FilterBean

class FilterAdapter(val context: Context, val filterList: List<FilterBean>, val onFilterSelect: (Int) -> Unit) : RecyclerView.Adapter<FilterAdapter.FilterViewHolder>() {

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): FilterViewHolder {
        val view = LayoutInflater.from(context).inflate(R.layout.item_filter_list, parent, false)
        return FilterViewHolder(view)
    }

    override fun getItemCount(): Int {
        return filterList.size
    }

    override fun onBindViewHolder(holder: FilterViewHolder, position: Int) {
        holder.tvFilter.text = filterList[position].filterName
        holder.itemView.setOnClickListener {
            onFilterSelect.invoke(position)
        }
    }


    class FilterViewHolder(view: View) : RecyclerView.ViewHolder(view) {
        val tvFilter = view.findViewById<TextView>(R.id.tv_filter)

    }
}