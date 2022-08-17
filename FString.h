#pragma once

#include <array>
#include <iostream>
#include <charconv>


namespace Cotto{ // String fixed

using std::cout;
using std::cerr;
using std::endl;

using CStr = const char*;

inline constexpr size_t NultermIndexCStr(CStr str){ //like strlen
    size_t len{0}; //can by charPtrIncrement
    for(;; ++len){ if(str[len] == '\0'){ break; } }
    return len;
}

template<typename InIt, typename OutIt> inline constexpr OutIt Copy(InIt begin, InIt end, OutIt destFirst){
    for(; begin != end; ++begin, ++destFirst){ *destFirst = *begin; }
    return destFirst; //while(begin != end){ *destFirst++ = *begin++; }
}
template<typename FwdIt, typename T> constexpr void Fill(FwdIt begin, FwdIt end, const T& value){
    for(; begin != end; ++begin){ (*begin) = value; }
}

struct FString{
    using CharType = char;
    using SizeType = uint8_t;
    using PrintSizeType = int16_t;
    static constexpr size_t MaxSize{14}, MaxSizeNulled{MaxSize+1};
    static constexpr bool FillEmptyZero{false};
    std::array<CharType, MaxSizeNulled> array;
    uint8_t size; //private like

    template<size_t N> constexpr FString(const CharType (&arr)[N]) noexcept : array{}{
        static_assert(N <= MaxSizeNulled, "Must be enough size with null-terminator");
        Copy(arr, arr + N, array.begin());
        size = N - 1;
        array[size] = '\0';
        if constexpr(FillEmptyZero && N < MaxSizeNulled){ Fill(array.begin() + N, array.end(), '\0'); }
    }


    [[nodiscard]] constexpr bool IsEmpty() const noexcept{ return size == 0; }
    [[nodiscard]] constexpr bool IsEndValid() const noexcept{ return array.back() == '\0'; }
    [[nodiscard]] constexpr bool IsNultermValid() const noexcept{ return array[size] == '\0'; }

    [[nodiscard]] constexpr auto Str() const noexcept{ return array.cbegin(); }
    [[nodiscard]] constexpr auto Data() noexcept{ return array.data(); }
    [[nodiscard]] constexpr auto Capacity() const noexcept{ return array.size(); }
    [[nodiscard]] constexpr auto NultermedSize() const noexcept{ return array.size(); }
    constexpr void Clear() noexcept{ size = 0; }


    [[nodiscard]] constexpr auto begin() noexcept{ return array.begin(); }
    [[nodiscard]] constexpr auto begin() const noexcept{ return array.begin(); }
    [[nodiscard]] constexpr auto cbegin() const noexcept{ return array.cbegin(); }

    [[nodiscard]] constexpr auto end() noexcept{ return array.begin() + size; }
    [[nodiscard]] constexpr auto end() const noexcept{ return array.begin() + size; }
    [[nodiscard]] constexpr auto cend() const noexcept{ return array.cbegin() + size; }

    [[nodiscard]] constexpr auto contentEnd() noexcept{ return array.begin() + MaxSize; }


    [[nodiscard]] constexpr bool operator==(const FString& rhs) const{
        if(size != rhs.size){ return false; }
        for(size_t i{0}; i < size; ++i){ if(array[i] != rhs.array[i]){ return false; } }
        return true;
    }
    [[nodiscard]] constexpr bool operator!=(const FString& rhs) const{ return !(operator==(rhs)); }

    [[nodiscard]] constexpr bool operator<(const FString& rhs) const{
        if(size != rhs.size){ return false; } //check first noneq elem
        for(size_t i{0}; i < size; ++i){ if(array[i] != rhs.array[i]){ return array[i] < rhs.array[i]; } }
        return false; //bcs can be ==,
    }
    [[nodiscard]] constexpr bool operator<=(const FString& rhs) const{
        if(size != rhs.size){ return false; }
        for(size_t i{0}; i < size; ++i){ if(array[i] > rhs.array[i]){ return false; } }
        return true;
    }

    [[nodiscard]] constexpr bool operator>(const FString& rhs) const{
        if(size != rhs.size){ return false; } //check first noneq elem
        for(size_t i{0}; i < size; ++i){ if(array[i] != rhs.array[i]){ return array[i] > rhs.array[i]; } }
        return false; //bcs can be ==
    }
    [[nodiscard]] constexpr bool operator>=(const FString& rhs) const{
        if(size != rhs.size){ return false; }
        for(size_t i{0}; i < size; ++i){ if(array[i] < rhs.array[i]){ return false; } }
        return true;
    }


    constexpr FString& AddTo(CharType val) noexcept{
        if(size > MaxSize){ cerr<<__PRETTY_FUNCTION__<<" : oversized, size:"<<PrintSizeType(size)<<endl; return *this; }
        array[size++] = val;
        array[size] = '\0';
        return *this;
    }
    constexpr FString& AddTo(const FString& val) noexcept{
        const SizeType bothsize{static_cast<SizeType>(size + val.size)};
        if(bothsize > MaxSize){ cerr<<__PRETTY_FUNCTION__<<" : oversized, size:"<<PrintSizeType(size)<<endl; return *this; }
        Copy(val.begin(), val.end(), end());
        size = bothsize;
        array[size] = '\0';
        return *this;
    }
    template<size_t N> constexpr FString& AddTo(const CharType (&val)[N]) noexcept{ //to avoid "strlen" call
        static_assert(N <= MaxSizeNulled, "Must be enough size with null-terminator");
        const SizeType valsize{N - 1};
        const SizeType bothsize{static_cast<SizeType>(size + valsize)};
        if(bothsize > MaxSize){ cerr<<__PRETTY_FUNCTION__<<" : oversized, size:"<<PrintSizeType(size)<<endl; return *this; }
        Copy(val, val + valsize, end());
        size = bothsize;
        array[size] = '\0';
        return *this;
    }
    template<typename Integer> constexpr FString& AddTo(Integer val) noexcept{
        if((MaxSize - size) < 1){ cerr<<__PRETTY_FUNCTION__<<" : already full"<<endl; return *this; }
        const auto convres{std::to_chars(end(), contentEnd(), val)};
        if(convres.ec != std::errc{}){ cerr<<__PRETTY_FUNCTION__<<" : bad conv"<<endl; return *this; }
        const auto dist{std::distance(end(), convres.ptr)};
        size += dist;
        array[size] = '\0';
        return *this;
    }
    constexpr FString& AddTo(float val) noexcept{ //DANGEROUS, future need make by inner logic
        if((MaxSize - size) < 3){ cerr<<__PRETTY_FUNCTION__<<" : already full"<<endl; return *this; } //bcs 0.0 minumum
        return AddTo(static_cast<size_t>(val)).AddTo('.').AddTo(static_cast<size_t>(val * 10) % 10); //10 as one decimal point
    }
    constexpr FString& AddTo(double val) noexcept{ //CLONE of float overload
        if((MaxSize - size) < 3){ cerr<<__PRETTY_FUNCTION__<<" : already full"<<endl; return *this; } //bcs 0.0 minumum
        return AddTo(static_cast<size_t>(val)).AddTo('.').AddTo(static_cast<size_t>(val * 10) % 10); //10 as one decimal point
    }

    [[nodiscard]] constexpr FString AddNew(CharType val) noexcept{ return FString{*this}.AddTo(val); }
    [[nodiscard]] constexpr FString AddNew(const FString& val) noexcept{ return FString{*this}.AddTo(val); }
    template<size_t N> [[nodiscard]] constexpr FString AddNew(const CharType (&val)[N]) noexcept{ return FString{*this}.AddTo(val); }
    template<typename Integer> [[nodiscard]] constexpr FString AddNew(Integer val) noexcept{ return FString{*this}.AddTo(val); }
    [[nodiscard]] constexpr FString AddNew(float val) noexcept{ return FString{*this}.AddTo(val); } //one digit WITHOUT small adition
    [[nodiscard]] constexpr FString AddNew(double val) noexcept{ return FString{*this}.AddTo(val); } //one digit WITHOUT small adition

    template<size_t N> [[nodiscard]] FString PreAddNew(const CharType (&val)[N]) noexcept{ //simple work non-atomic
        static_assert(N <= MaxSizeNulled, "Must be enough size with null-terminator"); //WITH nultemr by strarr-size
        return FString{val}.AddTo(*this);
    }

    [[nodiscard]] static constexpr FString FromStr(CStr str, size_t size) noexcept{ //size == nultermPos, must have nulterm
        FString result{""};
        if(size > MaxSize){ cerr<<__PRETTY_FUNCTION__<<": oversized, get:"<<PrintSizeType(size)<<", max:"<<MaxSize<<endl; return result; }
        Copy(str, str + size, result.begin());
        result.size = static_cast<SizeType>(size);
        result.array[size] = '\0';
        if constexpr(FillEmptyZero){ if(size < MaxSize){ Fill(result.end(), result.array.end(), '\0'); } }
        return result; //can FromCStr: call my-strlen to size
    }
    [[nodiscard]] static FString FromStdString(const std::string& str) noexcept{ //NEED TO AVOUD, bcs std::string non-constexpr at all
        return FromStr(str.c_str(), str.size()); //size == length == pos of nulterm, c_str == data
    }
};

inline std::ostream& operator<<(std::ostream& os, const FString& str) noexcept{ return os<<str.Str(); }


//==== NEXT for included ImGui.h with using namespace ImGui (InputTextF and TextPlain calls) ====

//inline int InputTextFStringCallback(ImGuiInputTextCallbackData* data){
//    if(data->EventFlag != ImGuiInputTextFlags_CallbackEdit){ return 0; } //can assert
//    FString* str{reinterpret_cast<FString*>(data->UserData)};
//    str->size = static_cast<FString::SizeType>(data->BufTextLen); //exclude nulterm (instead bufsize)
//    return 0; //bcs all not-chained paths return 0 //can't check any, bcs buf update after this fn call
//}
//inline bool InputTextF(CStr label, FString& str, ImGuiInputTextFlags flags = ImGuiInputTextFlags_None){
//    IM_ASSERT((flags & ImGuiInputTextFlags_CallbackEdit) == 0); //and no resize
//    flags |= ImGuiInputTextFlags_CallbackEdit;
//    return InputText(label, str.Data(), str.NultermedSize(), flags, InputTextFStringCallback, &str);
//}

//inline void TextPlain(const FString& str){ TextEx(str.Str(), str.end(), ImGuiTextFlags_NoWidthForLargeClippedText); }

} // namespace Cotto
